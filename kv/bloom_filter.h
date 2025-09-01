#pragma once
#include <vector>
#include <cstdint>
#include "xxhash.h"
#include "types.h"

namespace kv {

class BloomFilter {
public:
    BloomFilter(size_t num_items, double false_positive_rate) {
        size_t bits = static_cast<size_t>(-1.44 * num_items * log(false_positive_rate) / log(2.0));
        num_hashes_ = static_cast<int>(bits / num_items * log(2.0));
        if (num_hashes_ < 1) num_hashes_ = 1;
        if (num_hashes_ > 30) num_hashes_ = 30;
        bitset_.resize((bits + 7) / 8, 0);
    }

    void add(const Key& key) {
        for (int i = 0; i < num_hashes_; ++i) {
            uint64_t hash = XXH3_64bits_withSeed(&key, sizeof(key), i);
            size_t bit_index = hash % (bitset_.size() * 8);
            bitset_[bit_index / 8] |= (1 << (bit_index % 8));
        }
    }

    bool might_contain(const Key& key) const {
        for (int i = 0; i < num_hashes_; ++i) {
            uint64_t hash = XXH3_64bits_withSeed(&key, sizeof(key), i);
            size_t bit_index = hash % (bitset_.size() * 8);
            if (!(bitset_[bit_index / 8] & (1 << (bit_index % 8)))) {
                return false;
            }
        }
        return true;
    }

    const std::vector<uint8_t>& get_data() const {
        return bitset_;
    }

private:
    std::vector<uint8_t> bitset_;
    int num_hashes_;
};

} // namespace kv