// citybits.cpp
// Fixed WAL writer: stable record format, correct checksum, batching, fdatasync.

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#ifdef __APPLE__
  #include <sys/fcntl.h>   // F_FULLFSYNC
#endif

#include "xxhash.h"

// ---------------- kv types (unchanged layout) ----------------
namespace kv {

struct Key {
    uint16_t zone_id;
    uint16_t object_type;
    uint16_t x;
    uint16_t y;
};
static_assert(sizeof(Key) == 8, "Key must be 8 bytes");

struct Value {
    uint64_t data;
};
static_assert(sizeof(Value) == 8, "Value must be 8 bytes");

struct WALHeader {
    static constexpr uint32_t MAGIC = 0xDEADBEEF;
    static constexpr uint16_t VERSION = 1;

    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint64_t checksum; // XXH3 over [key][value]
};

struct WALRecord {
    WALHeader header;
    Key key;
    Value value;
};

} // namespace kv

// -------------- little-endian helpers (portable) --------------
static inline uint16_t bswap16(uint16_t x){ return uint16_t((x>>8)|(x<<8)); }
static inline uint32_t bswap32(uint32_t x){
    return (x>>24) | ((x>>8)&0x0000FF00u) | ((x<<8)&0x00FF0000u) | (x<<24);
}
static inline uint64_t bswap64(uint64_t x){
    return  (x>>56)
          | ((x>>40)&0x000000000000FF00ull)
          | ((x>>24)&0x0000000000FF0000ull)
          | ((x>>8) &0x00000000FF000000ull)
          | ((x<<8) &0x000000FF00000000ull)
          | ((x<<24)&0x0000FF0000000000ull)
          | ((x<<40)&0x00FF000000000000ull)
          | (x<<56);
}
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__)
  static inline uint16_t to_le16(uint16_t x){ return x; }
  static inline uint32_t to_le32(uint32_t x){ return x; }
  static inline uint64_t to_le64(uint64_t x){ return x; }
#else
  static inline uint16_t to_le16(uint16_t x){ return bswap16(x); }
  static inline uint32_t to_le32(uint32_t x){ return bswap32(x); }
  static inline uint64_t to_le64(uint64_t x){ return bswap64(x); }
#endif

// On-disk 16B header (always written in little-endian)
struct HeaderOnDisk {
    uint32_t magic_le;    // 0..3
    uint16_t version_le;  // 4..5
    uint16_t reserved_le; // 6..7 (0)
    uint64_t checksum_le; // 8..15
};
static_assert(sizeof(HeaderOnDisk) == 16, "HeaderOnDisk must be 16 bytes");

// ----------------------- WAL Writer --------------------------
class WALWriter {
public:
    explicit WALWriter(const std::string& path,
                       size_t batch_bytes = (1u << 20)) // 1 MiB default
        : path_(path), batch_bytes_(batch_bytes) {
        fd_ = ::open(path.c_str(), O_CREAT | O_APPEND | O_WRONLY | O_CLOEXEC, 0644);
        if (fd_ < 0) throw std::runtime_error("open WAL failed");
        buffer_.reserve(batch_bytes_);
    }

    ~WALWriter() {
        // Best-effort durable flush
        (void)flush(true);
        if (fd_ >= 0) ::close(fd_);
    }

    // Append one record: [header][key][value] (32 bytes total)
    bool append(const kv::Key& key, const kv::Value& value) {
        // 1) Compute checksum over *serialized payload* [key][value]
        XXH3_state_t* st = XXH3_createState();
        if (!st) return false;
        XXH3_64bits_reset(st);
        XXH3_64bits_update(st, &key,   sizeof(kv::Key));
        XXH3_64bits_update(st, &value, sizeof(kv::Value));
        uint64_t checksum = XXH3_64bits_digest(st);
        XXH3_freeState(st);

        // 2) Build little-endian header
        HeaderOnDisk hdr{
            to_le32(kv::WALHeader::MAGIC),
            to_le16(kv::WALHeader::VERSION),
            to_le16(0u),
            to_le64(checksum)
        };

        // 3) Serialize to buffer: [hdr][key][value]
        const size_t rec_sz = sizeof(HeaderOnDisk) + sizeof(kv::Key) + sizeof(kv::Value);
        const size_t pos = buffer_.size();
        buffer_.resize(pos + rec_sz);
        std::memcpy(buffer_.data() + pos,                               &hdr,   sizeof(hdr));
        std::memcpy(buffer_.data() + pos + sizeof(hdr),                 &key,   sizeof(kv::Key));
        std::memcpy(buffer_.data() + pos + sizeof(hdr) + sizeof(kv::Key), &value, sizeof(kv::Value));

        // 4) Batch threshold → flush + sync
        if (buffer_.size() >= batch_bytes_) {
            return flush(true);
        }
        return true;
    }

    // Flush pending bytes; if do_sync=true, make them durable
    bool flush(bool do_sync) {
        if (!buffer_.empty()) {
            if (!write_all(buffer_.data(), buffer_.size())) return false;
            buffer_.clear();
        }
        if (do_sync) {
        #ifdef __APPLE__
            if (::fcntl(fd_, F_FULLFSYNC) != 0) return false; // stronger durability on APFS/HFS+
        #else
            if (::fdatasync(fd_) != 0) return false;
        #endif
        }
        return true;
    }

private:
    bool write_all(const void* p, size_t n) {
        const uint8_t* b = static_cast<const uint8_t*>(p);
        size_t off = 0;
        while (off < n) {
            ssize_t rc = ::write(fd_, b + off, n - off);
            if (rc < 0) {
                if (errno == EINTR) continue;
                return false;
            }
            off += static_cast<size_t>(rc);
        }
        return true;
    }

    std::string path_;
    int fd_{-1};
    std::vector<uint8_t> buffer_;
    size_t batch_bytes_;
};

// -------------------------- Demo REPL ------------------------
int main() {
    std::cout << "Hello, citybits storage engine!" << std::endl;

    WALWriter wal("citybits.wal", 256 * 1024); // 256 KiB batch
    std::string action_command;
    uint64_t seq = 1; // monotonic for easy hexdump/crash tests

    for (;;) {
        std::cout << "> ";
        if (!std::getline(std::cin, action_command) || action_command == "exit") break;

        if (action_command == "build") {
            kv::Key   k{1, 100, 12, 34};
            kv::Value v{seq++};
            if (!wal.append(k, v)) std::cerr << "append failed\n";
        } else if (action_command == "del") {
            kv::Key   k{1, 100, 12, 34};
            kv::Value tomb{0};
            if (!wal.append(k, tomb)) std::cerr << "append failed\n";
        } else if (action_command == "sync") {
            if (!wal.flush(true)) std::cerr << "flush failed\n";
        } else if (action_command == "spam") {
            for (int i = 0; i < 10000; ++i) {
                kv::Key k{1, 100, (uint16_t)(i%256), (uint16_t)((i/256)%256)};
                kv::Value v{seq++};
                if (!wal.append(k, v)) { std::cerr << "append failed\n"; break; }
            }
            if (!wal.flush(true)) std::cerr << "flush failed\n";
            std::cout << "synced up to seq " << (seq-1) << "\n";
        } else if (!action_command.empty()) {
            std::cout << "unknown command: " << action_command << std::endl;
        }
    }

    (void)wal.flush(true); // durable on clean exit
    return 0;
}
