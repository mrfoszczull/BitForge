<div align="center">

# BitForge

**A lightweight, portable C17 library for bit-level manipulation, sequential stream parsing, bit pattern matching, binary data handling, file I/O, and binary format parsers.**

[![CI](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml/badge.svg)](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard_revision))

</div>

---

BitForge provides a consistent C17 API for bitwise operations, bit-level streams, pattern matching, binary buffer manipulation, file I/O, endian-aware parsing helpers, and format-specific parsers (**PNG**, **BMP**, **WAV**, **ELF**, and **PE / PE32+**). It focuses on reliability, portability, explicit status codes, zero-allocation parsing paths where applicable, and predictable resource management.

## 📑 Table of Contents

- [Features](#-features)
- [Requirements](#-requirements)
- [Getting Started](#-getting-started)
- [CMake Integration](#-cmake-integration)
- [Documentation](#-documentation)
- [Testing and Static Analysis](#-testing-and-static-analysis)
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
- **File I/O** – binary file handling with open, close, read, write, seek, position, and file size operations.
- **Flexible file access** – support for read, write, append, and read/write modes.
- **Endian-aware parser helpers** – bounded reads for unsigned/signed integers (`u8`–`u64`, `i8`–`i64`), IEEE 754 floats (`f32`, `f64`), strings, bitfields, signature checks, and diagnostics.
- **Format parsers** – parsers for **PNG** (signature, `IHDR`, chunks, CRC-32), **BMP** (`BM`, file/DIB headers, row stride, pixel view), **WAV** (`RIFF`/`WAVE`, subchunks, `fmt `, `data` slice), **ELF** (ELF32/ELF64, LE/BE, program/section headers, symbols, relocations), and **PE / PE32+** (DOS/MZ, COFF, PE32/PE32+ optional headers, data directories, sections, RVA translation).
- **Bit analysis** – count set bits and check whether a value is a power of two.
- **Error handling** – explicit `bf_status` codes for predictable error handling.
- **Memory control** – caller-owned buffers and zero-copy slice views over stream data without mandatory dynamic allocation.
- **Resource management** – explicit file handle lifecycle and predictable cleanup.
- **Lightweight** – no external runtime dependencies.
- **Portable** – C17-compatible API designed for Linux, Windows, and macOS.
- **CMake integration** – installable static library with exported CMake package support.

## 📋 Requirements

- C compiler with C17 support
- CMake 3.20 or newer
- Make *(optional, for the provided Makefile)*
- `cppcheck` *(optional, for static analysis in `make check`)*
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
| Public headers | `include/bitforge/` (including `include/bitforge/formats/`) |
| CMake package files | `lib/cmake/BitForge/` |

> 💡 To install BitForge system-wide, use an appropriate installation prefix for your platform.

## 🔗 CMake Integration

After installing BitForge, use `find_package()` to locate the package and link against its exported target:

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyApplication LANGUAGES C)

find_package(BitForge 2.0 CONFIG REQUIRED)

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
#include <bitforge/io.h>
#include <bitforge/parser.h>
#include <bitforge/formats/png.h>
#include <bitforge/formats/bmp.h>
#include <bitforge/formats/wav.h>
#include <bitforge/formats/elf.h>
#include <bitforge/formats/pe.h>
```

## 📚 Documentation

The API overview and conventions are available in **[docs/](docs/README.md)**, with module pages covering core and format parsers:

| Module | Header | Description |
|--------|--------|-------------|
| [bitops](docs/bitops.md) | `bitforge/bitops.h` | Bit-level operations on 8-bit values |
| [stream](docs/stream.md) | `bitforge/stream.h` | Sequential bit stream processing and parsing |
| [pattern](docs/pattern.md) | `bitforge/pattern.h` | Bit-level pattern matching with wildcard support |
| [buffer](docs/buffer.md) | `bitforge/buffer.h` | Caller-owned buffers and read-only memory views |
| [io](docs/io.md) | `bitforge/io.h` | Binary file operations and file management |
| [parser](docs/parser.md) | `bitforge/parser.h` | Endian-aware integers, floats, bitfields, strings, checks, and diagnostics |
| [status](docs/status.md) | `bitforge/status.h` | Status codes and error handling |
| [png](docs/formats_png.md) | `bitforge/formats/png.h` | PNG signature, `IHDR`, chunks, and CRC-32 |
| [bmp](docs/formats_bmp.md) | `bitforge/formats/bmp.h` | BMP signature, file/DIB headers, row stride, and pixel slice |
| [wav](docs/formats_wav.md) | `bitforge/formats/wav.h` | RIFF/WAVE container, chunks, `fmt `, and audio data slice |
| [elf](docs/formats_elf.md) | `bitforge/formats/elf.h` | ELF32/ELF64 headers, segments, sections, symbols, and relocations |
| [pe](docs/formats_pe.md) | `bitforge/formats/pe.h` | DOS, COFF, PE32/PE32+ optional headers, directories, sections, and RVAs |

> All functions return a `bf_status` value to indicate success or failure, unless their return type specifies otherwise.

To regenerate the module pages from Doxygen comments:

```bash
make docs
```

## 🧪 Testing and Static Analysis

Run the test suite with CTest:

```bash
ctest --test-dir build --output-on-failure
```

Or use the provided Makefile:

```bash
make test    # run tests
make check   # run build, sanitizer tests, cppcheck, and format-check
```

The test suite covers core modules (`bitops`, `stream`, `pattern`, `buffer`, `io`, `status`, `parser`) as well as format parsers (`png`, `bmp`, `wav`, `elf`, `pe`).

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

**BitForge v2.0.0** expands the library with endian-aware `parser` utilities and built-in binary format parsers across two major categories:

- **Media and container formats**: `png`, `bmp`, and `wav` parsers.
- **Executable and object formats**: `elf` and `pe` / `pe32+` parsers.

Buffer operations and format parsers avoid mandatory dynamic allocation, relying on caller-provided buffers, explicit bounds and overflow checks, and zero-copy slices where appropriate, while file handles remain explicitly managed via `bf_file_t`.

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
