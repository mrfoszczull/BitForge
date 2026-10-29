cmake_minimum_required(VERSION 3.20)

project(BitForgeExternalTest LANGUAGES C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(../.. bitforge-build)

add_executable(external_test main.c)
target_link_libraries(external_test PRIVATE bitforge)
