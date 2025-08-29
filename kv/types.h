#pragma once
#include "xxhash.h"
#include <cstdint>

namespace kv {

// A 64-bit key for our key-value store.
struct Key {
    uint16_t zone_id;
    uint16_t object_type;
    uint16_t x;
    uint16_t y;
};

struct Value {
    uint64_t data;
};

// The header for a single record in the Write-Ahead Log.
struct WALHeader {
    static constexpr uint32_t MAGIC = 0xDEADBEEF; // lol 
    static constexpr uint16_t VERSION = 1;

    uint32_t magic;
    uint16_t version;
    uint16_t reserved; 
    uint64_t checksum; /
};

// Represents a complete record in the WAL.
struct WALRecord {
    WALHeader header;
    Key key;
    Value value;
};

} // namespace kv