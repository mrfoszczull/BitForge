 <div align="center">

# BitForge

### Precision at the bit level.

**A lightweight, portable C17 library for bit manipulation, binary parsing, stream processing, and low-level data handling.**

[![CI](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml/badge.svg)](https://github.com/mrfoszczull/BitForge/actions/workflows/ci.yml)
[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)
[![C Standard](https://img.shields.io/badge/C-C17-blue.svg)](https://en.wikipedia.org/wiki/C17_(C_standard_revision))

</div>

---

BitForge is a lightweight C17 library designed for reliable bit-level operations and binary data processing.

It provides a consistent API for bit manipulation, sequential stream parsing, pattern matching, buffer management, binary file I/O, endian-aware parsing, and built-in parsers for popular binary formats.

**Supported formats:** PNG · BMP · WAV · ELF · PE / PE32+

Built with portability, explicit error handling, predictable resource management, and minimal runtime overhead in mind.

## Contents

- [Features](#features)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
- [CMake Integration](#cmake-integration)
- [Documentation](#documentation)
- [Testing & Quality](#testing--quality)
- [Project Status](#project-status)
- [Contributing](#contributing)
- [Security](#security)
- [License](#license)

---

## Features

BitForge is organized into several functional areas.

### Core Operations

| Module | Description |
|:---|:---|
| **Bit Manipulation** | Set, clear, toggle, and inspect individual bits and bit ranges. |
| **Bitwise Operations** | Reverse, rotate, and swap bits in 8-bit values. |
| **Bit Analysis** | Count set bits and check whether a value is a power of two. |
| **Bit Streams** | Sequential MSB-first bit reading across byte boundaries, skipping, and byte alignment. |
| **Pattern Matching** | Bit-level pattern searching with custom masks and wildcard support. |

### Binary Data Handling

| Module | Description |
|:---|:---|
| **Buffers** | Caller-owned memory buffers supporting read, write, append, clear, fill, copy, and comparison. |
| **Memory Views** | Zero-copy, read-only slices over existing binary data. |
| **File I/O** | Binary file operations including open, close, read, write, seek, position, and file size. |
| **File Access** | Read, write, append, and read/write modes. |
| **Parser Utilities** | Endian-aware bounded reads for integers, floating-point values, strings, and bitfields. |

### Binary Format Parsers

Built-in parsers provide structured access to binary file metadata and data regions.

| Format | Supported Features |
|:---|:---|
| **PNG** | Signature validation, IHDR, chunks, and CRC-32. |
| **BMP** | BM signature, file and DIB headers, row stride, and pixel views. |
| **WAV** | RIFF/WAVE container, subchunks, fmt, and data slices. |
| **ELF** | ELF32/ELF64, little-endian and big-endian, program and section headers, symbols, and relocations. |
| **PE** | DOS/MZ, COFF, PE32/PE32+ optional headers, data directories, sections, and RVA translation. |

### Design Principles

- **Portable** — C17-compatible API designed for Linux, Windows, and macOS.
- **Lightweight** — No external runtime dependencies.
- **Memory-conscious** — Caller-owned buffers and zero-copy parsing paths where applicable.
- **Predictable** — Explicit status codes and resource lifecycle management.
- **Safe parsing** — Bounded reads, explicit bounds validation, and overflow checks.
- **Consistent API** — Common conventions across core modules and format parsers.

---

## Requirements

| Dependency | Requirement |
|:---|:---|
| C Compiler | C17 support |
| CMake | 3.20 or newer |
| Make | Optional — provided Makefile |
| cppcheck | Optional — static analysis |
| ClangFormat | Optional — code formatting |
| Doxygen | Optional — API documentation generation |
| Moxygen | Optional — Markdown documentation generation |

---

## Getting Started

### 1. Clone the repository

```bash
git clone https://github.com/mrfoszczull/BitForge.git
cd BitForge
```

### 2. Configure and build

```bash
cmake -S . -B build
cmake --build build
```

### 3. Install

```bash
cmake --install build --prefix "$PWD/install"
```

### Installation Layout

The installation provides the following components:

| Component | Location |
|:---|:---|
| Static library | `lib/libbitforge.a` |
| Public headers | `include/bitforge/` |
| Format headers | `include/bitforge/formats/` |
| CMake package | `lib/cmake/BitForge/` |

> **Note:** To install BitForge system-wide, use an appropriate installation prefix for your platform.

---

## CMake Integration

BitForge exports a CMake package with the `BitForge::bitforge` target.

After installation, integrate it into your project using `find_package()`.

### CMakeLists.txt

```cmake
cmake_minimum_required(VERSION 3.20)

project(MyApplication LANGUAGES C)

find_package(BitForge 2.0 CONFIG REQUIRED)

add_executable(my_app main.c)

target_link_libraries(my_app PRIVATE BitForge::bitforge)
```

For a custom installation prefix:

```bash
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="/path/to/bitforge/install"
```

### Header Includes

Include only the modules required by your application:

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

---

## Documentation

Complete API documentation and module overviews are available in [`docs/`](docs/README.md).

### Core Modules

| Module | Header | Documentation |
|:---|:---|:---|
| Bit Operations | `bitforge/bitops.h` | [bitops.md](docs/bitops.md) |
| Bit Streams | `bitforge/stream.h` | [stream.md](docs/stream.md) |
| Pattern Matching | `bitforge/pattern.h` | [pattern.md](docs/pattern.md) |
| Binary Buffers | `bitforge/buffer.h` | [buffer.md](docs/buffer.md) |
| File I/O | `bitforge/io.h` | [io.md](docs/io.md) |
| Parser Utilities | `bitforge/parser.h` | [parser.md](docs/parser.md) |
| Status Codes | `bitforge/status.h` | [status.md](docs/status.md) |

### Binary Format Parsers

| Format | Header | Documentation |
|:---|:---|:---|
| PNG | `bitforge/formats/png.h` | [formats_png.md](docs/formats_png.md) |
| BMP | `bitforge/formats/bmp.h` | [formats_bmp.md](docs/formats_bmp.md) |
| WAV | `bitforge/formats/wav.h` | [formats_wav.md](docs/formats_wav.md) |
| ELF | `bitforge/formats/elf.h` | [formats_elf.md](docs/formats_elf.md) |
| PE | `bitforge/formats/pe.h` | [formats_pe.md](docs/formats_pe.md) |

> **API Convention:** Unless a function's return type specifies otherwise, functions return a `bf_status` value indicating success or failure.

### Regenerating Documentation

Generate module documentation from Doxygen comments:

```bash
make docs
```

---

## Testing & Quality

BitForge includes tests for its core functionality and supported binary format parsers.

### Running Tests

Using CTest:

```bash
ctest --test-dir build --output-on-failure
```

Or using the provided Makefile:

```bash
make test
```

The test suite covers:

- Core modules: `bitops`, `stream`, `pattern`, `buffer`, `io`, `status`, and `parser`.
- Format parsers: `png`, `bmp`, `wav`, `elf`, and `pe`.

### Static Analysis & Formatting

Run the complete project checks:

```bash
make check
```

This runs the build, sanitizer tests, cppcheck, and formatting checks.

Format source files:

```bash
make format
```

Check formatting without modifying files:

```bash
make format-check
```

### Sanitizers

AddressSanitizer and UndefinedBehaviorSanitizer can be enabled using the `BITFORGE_ENABLE_SANITIZERS` CMake option.

```bash
cmake -S . -B build-sanitize \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBITFORGE_ENABLE_SANITIZERS=ON

cmake --build build-sanitize

ctest --test-dir build-sanitize --output-on-failure
```

---

## Project Status

**Current release: `v2.0.0`**

Version 2.0.0 expands BitForge with endian-aware parser utilities and built-in binary format parsers.

### What's Included

| Category | Modules |
|:---|:---|
| Core functionality | Bit operations, streams, pattern matching, buffers |
| Binary I/O | File handling and binary data access |
| Parser utilities | Endian-aware integers, floats, strings, bitfields, and diagnostics |
| Media formats | PNG, BMP, WAV |
| Executable formats | ELF32/ELF64, PE32/PE32+ |

### Memory & Resource Management

Buffer operations and format parsers avoid mandatory dynamic allocation.

The library relies on:

- Caller-provided memory buffers.
- Explicit bounds and overflow checks.
- Zero-copy data slices where appropriate.
- Explicit file handle lifecycle management through `bf_file_t`.

This design provides predictable memory behavior and gives applications direct control over resource ownership.

---

## Contributing

Contributions, bug reports, and feature suggestions are welcome.

Before contributing, please review [`CONTRIBUTING.md`](docs/CONTRIBUTING.md) for development setup, coding guidelines, and the pull request process.

### Before Submitting a Pull Request

Run the following checks:

```bash
make format
make check
```

### Bug Reports & Feature Requests

[Open an issue](https://github.com/mrfoszczull/BitForge/issues).

Please include:

- BitForge version.
- Operating system and architecture.
- Compiler and version.
- A minimal reproducible example, when applicable.

---

## Security

If you discover a potential security vulnerability, **do not report it through public GitHub issues**.

Please submit a private report through GitHub Security Advisories.

See [`SECURITY.md`](docs/SECURITY.md) for the complete security policy, including reporting requirements and vulnerability handling procedures.

Security fixes are provided for the latest stable release.

---

## License

BitForge is distributed under the **MIT License**.

See [`LICENSE`](LICENSE) for details.

---

<div align="center">

**BitForge** · Lightweight. Portable. Precise.

<sub>Built for developers who work close to the bits.</sub>

</div>
