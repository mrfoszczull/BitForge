# BitForge API Reference

Reference documentation for the public API of BitForge. The module pages are generated from the Doxygen comments in `include/bitforge/` with `make docs`; this index is maintained by hand.

## Modules

| Module | Header | Description |
|--------|--------|-------------|
| [bitops](bitops.md) | `bitops.h` | Single-bit and bit-range operations on 8-bit values: get, set, clear, toggle, swap, reverse, rotate, count set bits, power-of-two check. |
| [stream](stream.md) | `stream.h` | Safe sequential reading of arbitrary bit counts across byte boundaries, with bit skipping and byte alignment. |
| [pattern](pattern.md) | `pattern.h` | Bit-level pattern matching and searching in a stream, with custom masks and wildcards. |
| [status](status.md) | `status.h` | Status codes (`bf_status`) returned by all BitForge functions. |

## Conventions

### Bit indexing and endianness

| Module | Order | Description |
|--------|-------|-------------|
| `bitops.h` | **LSB-first** | Bit index `0` is the least significant bit; index `7` is the most significant bit. |
| `stream.h`, `pattern.h` | **MSB-first** | Bits are read and matched most significant first across buffer boundaries, matching standard binary formats and protocol specifications. |

### Error handling

Every function returns a `bf_status` value. Check it against `BF_OK` before using any output parameter. See [status](status.md) for the full list of codes.

## Regenerating

```bash
make docs
```

This runs Doxygen and Moxygen and writes the module pages into this directory.

