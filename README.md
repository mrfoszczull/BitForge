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
- **Bit analysis** – count set bits and check whether a value is a power of two.
- **Error handling** – explicit status codes for predictable error handling.
- **Lightweight** – no external runtime dependencies.
- **Portable** – C17-compatible API.
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

find_package(BitForge 1.3 CONFIG REQUIRED)

add_executable(my_app main.c)

target_link_libraries(my_app PRIVATE BitForge::bitforge)
```

For a custom installation prefix, specify `CMAKE_PREFIX_PATH`:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="/path/to/bitforge/install"
```

Then include the headers you need, for example `<bitforge/bitops.h>` or `<bitforge/stream.h>`.

## 📚 Documentation

The full API reference, including bit indexing and endianness conventions, is in **[docs/](docs/README.md)**:

| Module | Description |
|--------|-------------|
| [bitops](docs/bitops.md) | Bit-level operations on 8-bit values |
| [stream](docs/stream.md) | Sequential bit stream processing and parsing |
| [pattern](docs/pattern.md) | Bit-level pattern matching with wildcard support |
| [status](docs/status.md) | Status codes and error handling |

> All functions return a `bf_status` value to indicate success or failure.

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
