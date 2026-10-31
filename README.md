# BitForge

BitForge is a lightweight C17 library for bit-level manipulation and analysis of 8-bit values.

It provides a simple and consistent API for common bitwise operations, with explicit status codes and a focus on portability, reliability, and ease of integration.

## Features

- Set, clear, toggle, and check individual bits.
- Perform bitwise operations on 8-bit values.
- Explicit status codes for error handling.
- Lightweight implementation with no external dependencies.
- C17-compatible API.
- CMake installation and package export support.

## Requirements

- A C compiler with C17 support.
- CMake 3.20 or newer.
- GNU Make (optional, for Makefile shortcuts).

## Getting Started

Clone the repository and enter the project directory:

```bash
git clone https://github.com/mrfoszczull/BitForge
cd BitForge
```

Configure and build the project with CMake:

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

- Static library: `lib/libbitforge.a`
- Public headers: `include/bitforge/`
- CMake package configuration: `lib/cmake/BitForge/`

You can also install BitForge system-wide by using an appropriate installation prefix for your system.

## Using BitForge in Another CMake Project

After installing BitForge, use `find_package()` to locate the package and link against its exported target.

Example `CMakeLists.txt`:

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyApplication LANGUAGES C)

find_package(BitForge 1.0 CONFIG REQUIRED)

add_executable(my_app main.c)

target_link_libraries(my_app PRIVATE BitForge::bitforge)
```

If BitForge was installed to a custom prefix, provide that prefix when configuring your project:

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

Run the test suite using CTest:

```bash
ctest --test-dir build --output-on-failure
```

If you use the provided Makefile, you can also run:

```bash
make test
```

Run both regular and sanitizer-enabled tests:

```bash
make check
```

## Code Formatting

BitForge uses ClangFormat to maintain consistent source code formatting.

Format the source files:

```bash
make format
```

Check formatting without modifying files:

```bash
make format-check
```

## Sanitizers

AddressSanitizer and UndefinedBehaviorSanitizer can be enabled through the CMake option:

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

BitForge 1.0.0 is released.

The public API is considered stable throughout the 1.x release series. Breaking API changes are reserved for a future major release.

## License

See the [LICENSE](LICENSE) file for license information.

