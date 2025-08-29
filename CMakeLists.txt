# Sets the minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Defines the project name and sets the C++ standard
project(citybits VERSION 1.0 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Adds the command-line application executable
# The executable will now be named "citybits"
add_executable(${PROJECT_NAME} cli/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE kv)

# Enable AddressSanitizer and UndefinedBehaviorSanitizer for debugging
set(SANITIZE_FLAGS "-fsanitize=address,undefined")
target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZE_FLAGS} -g)
target_link_libraries(${PROJECT_NAME} PRIVATE ${SANITIZE_FLAGS})