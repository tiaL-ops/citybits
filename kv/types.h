#pragma once
#include "xxhash.h"
#include <cstdint>
#include <vector>

namespace kv {

// A 64-bit key for our key-value store.
struct Key {
    uint16_t zone_id;
    uint16_t object_type;
    uint16_t x;
    uint16_t y;

    // For sorting keys in the MemTable and SSTable
    bool operator<(const Key& other) const {
        if (zone_id != other.zone_id) return zone_id < other.zone_id;
        if (object_type != other.object_type) return object_type < other.object_type;
        if (x != other.x) return x < other.x;
        return y < other.y;
    }
};
static_assert(sizeof(Key) == 8, "Key must be 8 bytes");


struct Value {
    uint64_t data;
};
static_assert(sizeof(Value) == 8, "Value must be 8 bytes");

// The header for a single record in the Write-Ahead Log.
struct WALHeader {
    static constexpr uint32_t MAGIC = 0xDEADBEEF;
    static constexpr uint16_t VERSION = 1;

    uint32_t magic;
    uint16_t version;
    uint16_t reserved;
    uint64_t checksum;
};

// Represents a complete record in the WAL.
struct WALRecord {
    WALHeader header;
    Key key;
    Value value;
};

// Represents the footer of an SSTable file.
struct SSTableFooter {
    static constexpr uint32_t MAGIC = 0x5354424C; // "STBL" in ASCII hex
    uint32_t magic; // <-- FIX: Added this member
    uint64_t bloom_filter_offset;
    uint64_t bloom_filter_size;
    uint64_t index_offset;
    uint64_t index_size;
    uint32_t crc; // Checksum for the footer itself
};

} // namespace kv