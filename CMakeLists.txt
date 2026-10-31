cmake_minimum_required(VERSION 3.20)

project(BitForge
    VERSION 1.0.0
    DESCRIPTION "A C17 library for bit-level manipulation and analysis"
    LANGUAGES C
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

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

add_library(BitForge::bitforge ALIAS bitforge)

target_include_directories(bitforge
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

set_target_properties(bitforge PROPERTIES
    EXPORT_NAME bitforge
)

include(CTest)

if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

install(
    TARGETS bitforge
    EXPORT BitForgeTargets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
    DIRECTORY include/bitforge
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
    EXPORT BitForgeTargets
    FILE BitForgeTargets.cmake
    NAMESPACE BitForge::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BitForge
)

configure_package_config_file(
    cmake/BitForgeConfig.cmake.in
    ${CMAKE_CURRENT_BINARY_DIR}/BitForgeConfig.cmake
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BitForge
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/BitForgeConfigVersion.cmake
    VERSION ${PROJECT_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES
        ${CMAKE_CURRENT_BINARY_DIR}/BitForgeConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/BitForgeConfigVersion.cmake
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/BitForge
)
