# Sets the minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Silence the FetchContent_Populate deprecation for this project
if(POLICY CMP0169)
  cmake_policy(SET CMP0169 OLD)
endif()

# Defines the project name and sets the C/C++ standards
project(citybits VERSION 1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------- xxhash via FetchContent (build from single C file) ----------
include(FetchContent)
FetchContent_Declare(
  xxhash
  GIT_REPOSITORY https://github.com/Cyan4973/xxHash.git
  GIT_TAG        v0.8.1
)

FetchContent_GetProperties(xxhash)
if(NOT xxhash_POPULATED)
  FetchContent_Populate(xxhash)

  # Build xxhash as a tiny static lib from its single C source.
  add_library(xxhash STATIC
    ${xxhash_SOURCE_DIR}/xxhash.c
  )
  target_include_directories(xxhash PUBLIC ${xxhash_SOURCE_DIR})
  set_target_properties(xxhash PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# ------------------ executable target -------------------
add_executable(${PROJECT_NAME} cli/main.cpp)
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(${PROJECT_NAME} PRIVATE xxhash)

# ------------------ Sanitizers (Clang/GCC) -------------------
# Enable AddressSanitizer and UndefinedBehaviorSanitizer for debugging
set(SANITIZE_FLAGS "-fsanitize=address,undefined")

# Apply to compile and link steps (link options, not libraries)
if(CMAKE_CXX_COMPILER_ID MATCHES "Clang|AppleClang|GNU")
  target_compile_options(${PROJECT_NAME} PRIVATE ${SANITIZE_FLAGS} -g)
  target_link_options(${PROJECT_NAME} PRIVATE ${SANITIZE_FLAGS})
endif()
