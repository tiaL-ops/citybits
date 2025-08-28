//Key should contain four uint16_t members.

//Value should be a simple "Plain Old Data" (POD) struct. A single uint64_t member would be perfect for that.

#pragma once // we want to avoid multiple inclusions of this header file

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
} 
