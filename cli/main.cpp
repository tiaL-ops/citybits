#include <iostream>
#include <string>
#include "kv/types.h"
int main() {
    std::cout << "Hello, citybits storage engine!" << std::endl;
    std::string action_command;

    for (;;) {
        std::getline(std::cin, action_command);
        if (!std::cin) break;          
        if (action_command == "exit") break;

        if (action_command == "zone") {
            std::cout << "zone command" << std::endl;
        } else if (action_command == "build") {
            std::cout << "build command" << std::endl;
        } else if (action_command == "del") {
            std::cout << "del command" << std::endl;
        } else if (!action_command.empty()) {
            std::cout << "unknown command: " << action_command << std::endl;
        }
    }

    return 0;
}
