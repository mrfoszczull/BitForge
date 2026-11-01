# BitForge

[![CI](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml/badge.svg)](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard))

**BitForge** is a lightweight, portable C17 library for bit-level manipulation and analysis of 8-bit values.

It provides a simple and consistent API for common bitwise operations, with explicit status codes and a focus on reliability, portability, and ease of integration.

## Features

- **Bit manipulation** – set, clear, toggle, and check individual bits.
- **Bitwise operations** – perform common operations on 8-bit values.
- **Error handling** – explicit status codes for predictable error handling.
- **Lightweight** – no external runtime dependencies.
- **Portable** – C17-compatible API.
- **CMake integration** – installable static library with exported CMake package support.

## Requirements

- A C compiler with C17 support.
- CMake 3.20 or newer.
- GNU Make (optional, for Makefile shortcuts).

## Getting Started

Clone the repository:

```bash
git clone https://github.com/mrfoszczull/BitForge.git
cd BitForge
```

Configure and build:

```bash
cmake -S . -B build
cmake --build build
```

## Installation

Install BitForge to a local directory:

```bash
cmake --install build --prefix "$PWD/install"
```

The installation includes:

| Component | Location |
|---|---|
| Static library | `lib/libbitforge.a` |
| Public headers | `include/bitforge/` |
| CMake package files | `lib/cmake/BitForge/` |

To install BitForge system-wide, use an appropriate installation prefix for your platform.

## CMake Integration

After installing BitForge, use `find_package()` to locate the package and link against its exported target.

Example `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyApplication LANGUAGES C)

find_package(BitForge 1.0 CONFIG REQUIRED)

add_executable(my_app main.c)

target_link_libraries(my_app PRIVATE BitForge::bitforge)
```

For a custom installation prefix, specify `CMAKE_PREFIX_PATH`:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="/path/to/bitforge/install"

cmake --build build
```

Include the public headers in your source files:

```c
#include <bitforge/bitops.h>
#include <bitforge/status.h>
```

## Testing

Run the test suite with CTest:

```bash
ctest --test-dir build --output-on-failure
```

Alternatively, use the provided Makefile:

```bash
make test
```

Run regular and sanitizer-enabled checks:

```bash
make check
```

## Code Formatting

BitForge uses **ClangFormat** to maintain consistent source code formatting.

Format the source files:

```bash
make format
```

Check formatting without modifying files:

```bash
make format-check
```

## Sanitizers

AddressSanitizer and UndefinedBehaviorSanitizer can be enabled with the `BITFORGE_ENABLE_SANITIZERS` CMake option.

Configure, build, and test a sanitizer-enabled build:

```bash
cmake -S . -B build-sanitize \
    -DBITFORGE_ENABLE_SANITIZERS=ON \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build build-sanitize
ctest --test-dir build-sanitize --output-on-failure
```

## API Overview

Public headers are located in `include/bitforge/`:

| Header | Description |
|---|---|
| `bitops.h` | Bit-level operations on 8-bit values |
| `status.h` | Status codes and error handling |

## Project Status

**Current version: 1.0.0**

BitForge 1.0.0 has been released. The public API is considered stable throughout the 1.x release series. Breaking API changes are reserved for a future major release.

The long-term goal is to evolve BitForge into a lightweight, portable C library for bit-level operations and binary data manipulation.

## License

BitForge is distributed under the **MIT License**. See [LICENSE](LICENSE) for details.
