cmake_minimum_required(VERSION 3.20)

project(BitForge
    VERSION 0.1.0
    DESCRIPTION "A C17 library for binary file manipulation and analysis"
    LANGUAGES C
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

add_library(bitforge STATIC
    src/status.c
)

target_include_directories(bitforge
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

