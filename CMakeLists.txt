cmake_minimum_required(VERSION 3.20)

project(BitForge
    VERSION 1.0.0
    DESCRIPTION "A C17 library for binary file manipulation and analysis"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()


option(BITFORGE_ENABLE_SANITIZERS "Enable AddressSanitizer and UBSan" OFF)

if(BITFORGE_ENABLE_SANITIZERS AND CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer -g)
    add_link_options(-fsanitize=address,undefined)
endif()

add_library(bitforge STATIC
    src/status.c
    src/bitops.c
)

target_include_directories(bitforge
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()
