# BitForge

[![CI](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml/badge.svg)](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard))

**BitForge** is a lightweight, portable C17 library for bit-level manipulation and analysis of 8-bit values.

It provides a simple and consistent API for common bitwise operations, with explicit status codes and a focus on reliability, portability, and ease of integration.

## Features

- **Bit manipulation** – set, clear, toggle, and check individual bits.
- **Bit range operations** – read, set, clear, and toggle ranges of bits.
- **Bitwise operations** – reverse, rotate, and swap bits in 8-bit values.
- **Bit analysis** – count set bits and check whether a value is a power of two.
- **Error handling** – explicit status codes for predictable error handling.
- **Lightweight** – no external runtime dependencies.
- **Portable** – C17-compatible API.
- **CMake integration** – installable static library with exported CMake package support.

## Bit Indexing

BitForge uses **LSB-first indexing** for bit positions.

Bit index `0` refers to the least significant bit (LSB), while bit index `7` refers to the most significant bit (MSB) of an 8-bit value.

For range operations, `bit_index` specifies the first bit and `bit_count` specifies the number of consecutive bits to process.

## Requirements

- C compiler with C17 support.
- CMake 3.20 or newer.
- Make (optional, for using the provided Makefile).
- ClangFormat (optional, for code formatting).

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

Install the library:

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

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyApplication LANGUAGES C)

find_package(BitForge 1.1 CONFIG REQUIRED)

add_executable(my_app main.c)

target_link_libraries(my_app PRIVATE BitForge::bitforge)
```

For a custom installation prefix, specify `CMAKE_PREFIX_PATH`:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="/path/to/bitforge/install"
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
    -DCMAKE_BUILD_TYPE=Debug \
    -DBITFORGE_ENABLE_SANITIZERS=ON

cmake --build build-sanitize
ctest --test-dir build-sanitize --output-on-failure
```

## API Overview

Public headers are located in `include/bitforge/`:

| Header | Description |
|---|---|
| `bitops.h` | Bit-level operations on 8-bit values |
| `status.h` | Status codes and error handling |

### Bit Operations

The `bitops.h` header provides the following operations:

| Function | Description |
|---|---|
| `bf_get` | Reads an individual bit |
| `bf_set` | Sets an individual bit |
| `bf_clear` | Clears an individual bit |
| `bf_toggle` | Toggles an individual bit |
| `bf_count_set_bits` | Counts set bits |
| `bf_is_power_of_two` | Checks whether a value is a power of two |
| `bf_reverse_bits` | Reverses bit order |
| `bf_rotate_left` | Rotates bits to the left |
| `bf_rotate_right` | Rotates bits to the right |
| `bf_get_bits` | Reads a range of bits |
| `bf_set_bits` | Sets a range of bits |
| `bf_clear_bits` | Clears a range of bits |
| `bf_toggle_bits` | Toggles a range of bits |
| `bf_swap_bits` | Swaps two bits |

All functions return a `bf_status` value to indicate success or failure.

## Project Status

**BitForge 1.1.0 is released.**

This release introduces additional bit manipulation operations, including bit rotations, range operations, and bit swapping.

The public API is intended to remain stable throughout the 1.x release series. Breaking API changes are reserved for a future major release.

The long-term goal is to evolve BitForge into a lightweight, portable C library for bit-level operations and binary data manipulation.

## Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for the development setup, coding guidelines and the pull request process.

Before submitting a pull request, run:

```bash
make format
make check
```

Found a bug or have an idea? Open an [issue](https://github.com/mrfoszczull/BitForge/issues) and include the BitForge version, your platform and compiler, and a minimal example that reproduces the problem.

## Security

If you believe you have found a security vulnerability, **do not open a public issue**. Report it privately through [GitHub Security Advisories](https://github.com/mrfoszczull/BitForge/security/advisories/new) instead.

Security fixes are provided for the latest stable release. See [SECURITY.md](SECURITY.md) for the full policy, including what to include in a report and how it is handled.

## License

BitForge is distributed under the **MIT License**. See [LICENSE](LICENSE) for details.
