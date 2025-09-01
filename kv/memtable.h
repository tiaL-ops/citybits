#pragma once
#include <map>
#include "types.h"

namespace kv {

class MemTable {
public:
    void put(const Key& key, const Value& value) {
        table_[key] = value;
    }

    bool get(const Key& key, Value& value) const {
        auto it = table_.find(key);
        if (it != table_.end()) {
            value = it->second;
            return true;
        }
        return false;
    }
    
    // For SSTable flushing
    const std::map<Key, Value>& get_all() const {
        return table_;
    }

    void clear() {
        table_.clear();
    }
    
    bool is_empty() const {
        return table_.empty();
    }

private:
    std::map<Key, Value> table_;
};

} // namespace kv