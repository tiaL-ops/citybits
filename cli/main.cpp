#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include "kv/types.h"
#include "xxhash.h"

// Function to write a single record to the Write-Ahead Log 
bool write_to_wal(const kv::Key& key, const kv::Value& value) {
    kv::WALRecord record;
    record.header.magic = kv.WALHeader::MAGIC;
    record.header.version = kv.WALHeader::VERSION;
    record.header.reserved = 0;
    record.key = key;
    record.value = value;

    // Calculate the checksum over the key and value.
    record.header.checksum = XXH3_64bits(&record.key, sizeof(kv::Key) + sizeof(kv::Value));

    // Open the log file for writing.
    std::ofstream wal_file("citybits.wal", std::ios::binary | std::ios::app);
    if (!wal_file) {
        std::cerr << "Error: Could not open citybits.wal for writing." << std::endl;
        return false;
    }

    // Write the entire record struct to the file.
    wal_file.write(reinterpret_cast<const char*>(&record), sizeof(kv::WALRecord));
    wal_file.close();

    std::cout << "Successfully wrote record to WAL." << std::endl;
    return true;
}

int main() {
    std::cout << "Hello, citybits storage engine!" << std::endl;
    std::string action_command;

    for (;;) {
        std::cout << "> ";
        std::getline(std::cin, action_command);
        if (!std::cin || action_command == "exit") {
            break;
        }

        if (action_command == "zone") {
            std::cout << "zone command" << std::endl;
        } else if (action_command == "build") {
            std::cout << "build command" << std::endl;
            kv::Key new_key = {1, 100, 12, 34};
            kv::Value new_value = {9999};
            write_to_wal(new_key, new_value);
        } else if (action_command == "del") {
            std::cout << "del command" << std::endl;
            kv::Key key_to_delete = {1, 100, 12, 34};
            kv::Value tombstone_value = {0}; // Using 0 to signify a delete.
            write_to_wal(key_to_delete, tombstone_value);
        } else if (!action_command.empty()) {
            std::cout << "unknown command: " << action_command << std::endl;
        }
    }

    return 0;
}