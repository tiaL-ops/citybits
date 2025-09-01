#pragma once
#include <string>
#include <vector>
#include <fstream>
#include "types.h"
#include "memtable.h"
#include "bloom_filter.h"
#include <iostream>

namespace kv {

class SSTableWriter {
public:
    static bool flush_memtable(const MemTable& memtable, const std::string& path) {
        std::ofstream out(path, std::ios::binary);
        if (!out) {
            std::cerr << "Failed to open SSTable file for writing: " << path << std::endl;
            return false;
        }

        const auto& data = memtable.get_all();
        
        // 1. Build and write Bloom Filter
        BloomFilter filter(data.size(), 0.01);
        for(const auto& pair : data){
            filter.add(pair.first);
        }
        const auto& bloom_data = filter.get_data();
        uint64_t bloom_offset = out.tellp();
        out.write(reinterpret_cast<const char*>(bloom_data.data()), bloom_data.size());
        
        // 2. Write data blocks
        for (const auto& pair : data) {
            out.write(reinterpret_cast<const char*>(&pair.first), sizeof(Key));
            out.write(reinterpret_cast<const char*>(&pair.second), sizeof(Value));
        }

        // 3. Footer
        SSTableFooter footer;
        footer.magic = SSTableFooter::MAGIC; // This line is now correct
        footer.bloom_filter_offset = bloom_offset;
        footer.bloom_filter_size = bloom_data.size();
        // For simplicity, we are not creating a separate index block in this version
        footer.index_offset = 0; 
        footer.index_size = 0;
        
        // Write footer
        out.write(reinterpret_cast<const char*>(&footer), sizeof(footer));
        
        out.close();
        return true;
    }
};

} // namespace kv