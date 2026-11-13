<div align="center">

# 🔧 BitForge

**A lightweight, portable C17 library for bit-level manipulation, sequential stream parsing, bit pattern matching, and binary data handling.**

[![CI](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml/badge.svg)](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard_revision))

</div>

---

BitForge provides a simple and consistent API for bitwise operations, bit-level streams, pattern matching, and binary buffer manipulation. It focuses on reliability, portability, explicit status codes, and predictable memory management.

## 📑 Table of Contents

- [Features](#-features)
- [Requirements](#-requirements)
- [Getting Started](#-getting-started)
- [CMake Integration](#-cmake-integration)
- [Documentation](#-documentation)
- [Testing](#-testing)
- [Code Formatting](#-code-formatting)
- [Sanitizers](#-sanitizers)
- [Project Status](#-project-status)
- [Contributing](#-contributing)
- [Security](#-security)
- [License](#-license)

## ✨ Features

- **Bit manipulation** – set, clear, toggle, and check individual bits and bit ranges.
- **Bitwise operations** – reverse, rotate, and swap bits in 8-bit values.
- **Bit stream processing** – safe sequential reading of arbitrary bit counts (MSB-first) across byte boundaries, with bit skipping and byte alignment.
- **Bit pattern matching** – efficient bit-level pattern matching and searching with custom masks and wildcards.
- **Binary buffers and views** – caller-owned memory buffers with read, write, append, clear, fill, copy, and comparison operations.
- **Bit analysis** – count set bits and check whether a value is a power of two.
- **Error handling** – explicit status codes for predictable error handling.
- **Memory control** – no mandatory dynamic allocation; memory is provided and managed by the caller.
- **Lightweight** – no external runtime dependencies.
- **Portable** – C17-compatible API designed for Linux, Windows, and macOS.
- **CMake integration** – installable static library with exported CMake package support.

## 📋 Requirements

- C compiler with C17 support
- CMake 3.20 or newer
- Make *(optional, for the provided Makefile)*
- ClangFormat *(optional, for code formatting)*
- Doxygen and Moxygen *(optional, for generating the API documentation)*

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

find_package(BitForge 1.5 CONFIG REQUIRED)

add_executable(my_app main.c)

target_link_libraries(my_app PRIVATE BitForge::bitforge)
```

For a custom installation prefix, specify `CMAKE_PREFIX_PATH`:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="/path/to/bitforge/install"
```

Then include the headers you need, for example:

```c
#include <bitforge/bitops.h>
#include <bitforge/stream.h>
#include <bitforge/pattern.h>
#include <bitforge/buffer.h>
```

## 📚 Documentation

The full API reference, including bit indexing, endianness conventions, buffer behavior, and memory requirements, is available in **[docs/](docs/README.md)**:

| Module | Description |
|--------|-------------|
| [bitops](docs/bitops.md) | Bit-level operations on 8-bit values |
| [stream](docs/stream.md) | Sequential bit stream processing and parsing |
| [pattern](docs/pattern.md) | Bit-level pattern matching with wildcard support |
| [buffer](docs/buffer.md) | Caller-owned buffers and read-only memory views |
| [status](docs/status.md) | Status codes and error handling |

> All functions return a `bf_status` value to indicate success or failure, unless their return type specifies otherwise.

To regenerate the module pages from the source comments:

```bash
make docs
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

## 📌 Project Status

**BitForge v1.5.0** is currently in active development.

Version 1.5.0 significantly extends the `bf_buffer` module with powerful new manipulation operations—including safe reading (`bf_buffer_read`), content insertion (`bf_buffer_insert`), element removal (`bf_buffer_remove`), dynamic resizing (`bf_buffer_resize`), and capacity reservation (`bf_buffer_reserve`). It builds upon the bit manipulation, sequential stream parsing, pattern matching, and binary buffer capabilities introduced in previous releases.

The library is designed around explicit memory ownership and does not require dynamic allocation for its core operations.

The public API is intended to remain stable throughout the 1.x release series. Breaking API changes are reserved for a future major release.

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](docs/CONTRIBUTING.md) for the development setup, coding guidelines, and the pull request process.

Before submitting a pull request, run:

```bash
make format
make check
```

Found a bug or have an idea? [Open an issue](https://github.com/mrfoszczull/BitForge/issues) and include the BitForge version, your platform and compiler, and a minimal example that reproduces the problem.

## 🔒 Security

If you believe you have found a security vulnerability, **do not open a public issue**. Report it privately through GitHub Security Advisories instead.

Security fixes are provided for the latest stable release. See [SECURITY.md](docs/SECURITY.md) for the full policy, including what to include in a report and how it is handled.

## 📄 License

BitForge is distributed under the MIT License. See [LICENSE](LICENSE) for details.
