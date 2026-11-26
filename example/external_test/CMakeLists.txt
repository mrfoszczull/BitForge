cmake_minimum_required(VERSION 3.20)

project(BitForgeExternalTest LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(BitForge 2.0.0 CONFIG REQUIRED)

add_executable(external_test main.c)
target_link_libraries(external_test PRIVATE BitForge::bitforge)
