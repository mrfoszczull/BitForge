# BitForge API Reference

Reference documentation for the public API of **BitForge 2.0.0**.

Module pages are generated from Doxygen comments in `include/bitforge/` using `make docs`. This index is maintained manually.

## Table of Contents

- [Modules](#modules)
  - [Core Modules](#core-modules)
  - [Format Parsers](#format-parsers)
- [Conventions](#conventions)
  - [Bit Indexing and Endianness](#bit-indexing-and-endianness)
  - [Buffer and Slice Model](#buffer-and-slice-model)
  - [File I/O Model](#file-io-model)
  - [Error Handling](#error-handling)
- [Regenerating Documentation](#regenerating-documentation)
- [Additional Resources](#additional-resources)

## Modules

### Core Modules

| Module | Header | Description |
|--------|--------|-------------|
| [bitops](bitops.md) | `bitforge/bitops.h` | Single-bit and bit-range operations on 8-bit values: get, set, clear, toggle, swap, reverse, rotate, count set bits, and power-of-two check. |
| [stream](stream.md) | `bitforge/stream.h` | Sequential MSB-first reading of arbitrary bit counts across byte boundaries, with bit skipping and byte alignment. |
| [pattern](pattern.md) | `bitforge/pattern.h` | Bit-level pattern matching and searching in a stream, with custom masks and wildcard support. |
| [buffer](buffer.md) | `bitforge/buffer.h` | Non-owning views and caller-provided buffers with bounded reading, slicing, writing, appending, clearing, filling, comparing, and copying operations. |
| [io](io.md) | `bitforge/io.h` | Binary file operations, including opening, closing, reading, writing, seeking, retrieving the current position, and querying file size. |
| [parser](parser.md) | `bitforge/parser.h` | Endian-aware unsigned/signed integers, IEEE 754 floats, strings, bitfields, signature/size validation, and diagnostics over `bf_stream_t`. |
| [status](status.md) | `bitforge/status.h` | Status codes (`bf_status`) returned by BitForge functions. |

### Format Parsers

| Module | Header | Description |
|--------|--------|-------------|
| [png](formats/png.md) | `bitforge/formats/png.h` | PNG 8-byte signature check, `IHDR` parsing, chunk stream reading, and ISO 3309 / IEEE 802.3 CRC-32 calculation and verification. |
| [bmp](formats/bmp.md) | `bitforge/formats/bmp.h` | BMP `"BM"` signature, 14-byte file header, DIB `BITMAPINFOHEADER`, 4-byte aligned row stride calculation, and zero-copy pixel slice view. |
| [wav](formats/wav.md) | `bitforge/formats/wav.h` | RIFF/WAVE header parsing, word-aligned RIFF subchunk reading, `fmt ` decoding, and zero-copy `"data"` sample slice view. |
| [elf](formats/elf.md) | `bitforge/formats/elf.h` | ELF32/ELF64 identification and headers (little/big-endian), program/section headers, extended indices, symbols, dynamic entries, relocations, and slices. |
| [pe](formats/pe.md) | `bitforge/formats/pe.h` | MZ/DOS header, `PE\0\0` signature and COFF header, PE32/PE32+ optional headers, data directories, section headers, RVA-to-file offset translation, and slices. |

## Conventions

### Bit Indexing and Endianness

Different modules use specific bit-ordering and byte-ordering conventions:

| Module / Scope | Bit / Byte Order | Description |
|----------------|------------------|-------------|
| `bitops.h` | **LSB-first** (8-bit) | Bit index `0` represents the least significant bit (`0x01`), while bit index `7` represents the most significant bit (`0x80`). |
| `stream.h`, `pattern.h` | **MSB-first** | Bits are read and matched from most significant to least significant across byte boundaries. |
| `parser.h` | **Explicit endianness** (`BF_ENDIAN_LITTLE` / `BF_ENDIAN_BIG`) | Multi-byte integer, float, and expectation helpers explicitly specify little-endian or big-endian byte order. |
| `buffer.h`, `io.h` | **Byte-oriented** | Operations work with bytes and byte offsets. |
| `formats/*` | **Format-defined** | Byte order follows each format specification (e.g., big-endian chunks/CRC in PNG, little-endian in BMP/WAV/PE, `EI_DATA` in ELF). |

### Buffer and Slice Model

Buffer operations do not allocate or free memory. The caller provides memory for `bf_buffer_t`, while `bf_view_t` provides read-only access to a byte range.

Format parsers operating over `bf_stream_t` also expose zero-copy slice/view structs (such as `bf_png_chunk_t`, `bf_bmp_pixels_t`, `bf_wav_chunk_t`, `bf_wav_data_t`, `bf_elf_slice_t`, and `bf_pe_slice_t`) whose `data` pointers refer directly into the backing stream buffer and remain valid only while that buffer stays alive and unmodified.

### File I/O Model

The I/O module provides binary file operations through the `bf_file_t` structure.

File resources are managed explicitly by the caller:

- Initialize a file object before opening it, for example, with `bf_file_t file = {0};`.
- Use `bf_io_open()` to open a file in the required mode.
- Use `bf_io_close()` to release an opened file.
- Use `bf_io_is_open()` to check whether a file object currently holds an open file handle.

Supported file modes:

| Mode | Description |
|------|-------------|
| `BF_FILE_READ` | Opens an existing file for reading. |
| `BF_FILE_WRITE` | Opens a file for writing, creating it if necessary or truncating it if it already exists. |
| `BF_FILE_APPEND` | Opens a file for appending, creating it if necessary. |
| `BF_FILE_READ_WRITE` | Opens an existing file for both reading and writing without truncating it. |

Read and write operations report the number of bytes processed through output parameters.

A read operation that reaches the end of a file may return `BF_OK` when some bytes were successfully read. `BF_EOF` indicates that no bytes could be read because the end of the file was reached.

File positioning uses absolute byte offsets relative to the beginning of the file.

### Error Handling

Every function returning a `bf_status` value uses explicit status codes to indicate success or failure (`BF_OK`, `BF_EINVAL`, `BF_EOF`, `BF_EFORMAT`, `BF_EOVERFLOW`, `BF_ENOTFOUND`, `BF_EUNSUPPORTED`, `BF_EIO`, `BF_ENOMEM`).

Always check the returned status against `BF_OK` before relying on output parameters.

See [status](status.md) for the complete list of available status codes and their meanings.

## Regenerating Documentation

The API reference can be regenerated by running the following command from the project root:

```bash
make docs
```

This runs Doxygen and Moxygen and writes the module pages into this directory.

## Additional Resources

- [Project README](../README.md)
- [Contributing Guidelines](CONTRIBUTING.md)
- [Security Policy](SECURITY.md)
