# Sets the minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Defines the project name and sets the C++ standard
project(citybits VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adds the command-line application executable
add_executable(city-sim cli/main.cpp)

# Enable AddressSanitizer and UndefinedBehaviorSanitizer for debugging
set(SANITIZE_FLAGS "-fsanitize=address,undefined")
target_compile_options(city-sim PRIVATE ${SANITIZE_FLAGS} -g)
target_link_libraries(city-sim PRIVATE ${SANITIZE_FLAGS})