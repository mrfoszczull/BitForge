<div align="center">

# 🔧 BitForge

**A lightweight, portable C17 library for bit-level manipulation, sequential stream parsing, bit pattern matching, and binary data analysis.**

[![CI](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml/badge.svg)](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard_revision))

</div>

---

BitForge provides a simple and consistent API for common bitwise operations, bit-level streams, and pattern matching, with explicit status codes and a focus on reliability, portability, and ease of integration.

## 📑 Table of Contents

- [Features](#-features)
- [Bit Indexing & Endianness](#-bit-indexing--endianness)
- [Requirements](#-requirements)
- [Getting Started](#-getting-started)
- [CMake Integration](#-cmake-integration)
- [Testing](#-testing)
- [Code Formatting](#-code-formatting)
- [Sanitizers](#-sanitizers)
- [API Overview](#-api-overview)
- [Project Status](#-project-status)
- [Contributing](#-contributing)
- [Security](#-security)
- [License](#-license)

## ✨ Features

- **Bit manipulation** – set, clear, toggle, and check individual bits.
- **Bit range operations** – read, set, clear, and toggle ranges of bits.
- **Bitwise operations** – reverse, rotate, and swap bits in 8-bit values.
- **Bit stream processing** – safe sequential reading of arbitrary bit counts (MSB-first) across byte boundaries, with support for bit skipping and byte alignment.
- **Bit pattern matching** – efficient bit-level pattern matching and searching with support for custom masks and wildcards (`bf_pattern`).
- **Bit analysis** – count set bits and check whether a value is a power of two.
- **Error handling** – explicit status codes for predictable error handling.
- **Lightweight** – no external runtime dependencies.
- **Portable** – C17-compatible API.
- **CMake integration** – installable static library with exported CMake package support.

## 🧭 Bit Indexing & Endianness

| Module | Header | Order | Description |
|--------|--------|-------|-------------|
| 8-bit value operations | `bitops.h` | **LSB-first** | Bit index `0` is the least significant bit (LSB); index `7` is the most significant bit (MSB). |
| Stream & pattern operations | `stream.h` / `pattern.h` | **MSB-first** | Bits are read and matched most significant first across buffer boundaries, matching standard binary formats and protocol specifications. |

## 📋 Requirements

- C compiler with C17 support
- CMake 3.20 or newer
- Make *(optional, for the provided Makefile)*
- ClangFormat *(optional, for code formatting)*

## 🚀 Getting Started

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
|-----------|----------|
| Static library | `lib/libbitforge.a` |
| Public headers | `include/bitforge/` |
| CMake package files | `lib/cmake/BitForge/` |

> 💡 To install BitForge system-wide, use an appropriate installation prefix for your platform.

## 🔗 CMake Integration

After installing BitForge, use `find_package()` to locate the package and link against its exported target:

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyApplication LANGUAGES C)

find_package(BitForge 1.3 CONFIG REQUIRED)

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
#include <bitforge/stream.h>
#include <bitforge/pattern.h>
#include <bitforge/status.h>
```

## 🧪 Testing

Run the test suite with CTest:

```bash
ctest --test-dir build --output-on-failure
```

Or use the provided Makefile:

```bash
make test    # run tests
make check   # run regular and sanitizer-enabled checks
```

## 🎨 Code Formatting

BitForge uses ClangFormat to maintain consistent source code formatting.

```bash
make format        # format source files
make format-check  # check formatting without modifying files
```

## 🛡️ Sanitizers

AddressSanitizer and UndefinedBehaviorSanitizer can be enabled with the `BITFORGE_ENABLE_SANITIZERS` CMake option.

```bash
cmake -S . -B build-sanitize \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBITFORGE_ENABLE_SANITIZERS=ON

cmake --build build-sanitize
ctest --test-dir build-sanitize --output-on-failure
```

## 📚 API Overview

Public headers are located in `include/bitforge/`:

| Header | Description |
|--------|-------------|
| `bitops.h` | Bit-level operations on 8-bit values |
| `stream.h` | Safe sequential bit stream processing and parsing |
| `pattern.h` | Bit-level pattern matching and searching with wildcard support |
| `status.h` | Status codes and error handling |

### Bit Operations (`bitops.h`)

| Function | Description |
|----------|-------------|
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

### Stream Operations (`stream.h`)

| Function | Description |
|----------|-------------|
| `bf_stream_init` | Initializes a bit stream over a buffer |
| `bf_stream_has_remaining` | Checks if a given number of bits remain |
| `bf_stream_read_bits` | Reads up to 32 bits (MSB-first) across byte boundaries |
| `bf_stream_skip_bits` | Skips a specified number of bits |
| `bf_stream_align` | Aligns the stream to the next byte boundary |
| `bf_stream_read_byte` | Reads a full byte (8 bits) |
| `bf_stream_read_bool` | Reads a single bit as a boolean value |

### Pattern Operations (`pattern.h`)

| Function | Description |
|----------|-------------|
| `bf_pattern_init` | Initializes a standard bit pattern from a byte buffer |

> All functions return a `bf_status` value to indicate success or failure.

## 📌 Project Status

BitForge v1.3.0 is currently in active development.

This version introduces the new `bf_pattern` module for efficient bit-level pattern matching and searching, building upon the robust sequential bit-stream parsing primitives of `bf_stream` from v1.2.0.

The public API is intended to remain stable throughout the 1.x release series. Breaking API changes are reserved for a future major release.

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for the development setup, coding guidelines, and the pull request process.

Before submitting a pull request, run:

```bash
make format
make check
```

Found a bug or have an idea? [Open an issue](https://github.com/mrfoszczull/BitForge/issues) and include the BitForge version, your platform and compiler, and a minimal example that reproduces the problem.

## 🔒 Security

If you believe you have found a security vulnerability, **do not open a public issue**. Report it privately through GitHub Security Advisories instead.

Security fixes are provided for the latest stable release. See [SECURITY.md](SECURITY.md) for the full policy, including what to include in a report and how it is handled.

## 📄 License

BitForge is distributed under the MIT License. See [LICENSE](LICENSE) for details.
