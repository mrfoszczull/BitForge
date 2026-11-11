# BitForge API Reference

Reference documentation for the public API of **BitForge**.

The module documentation is generated from Doxygen comments in `include/bitforge/` using `make docs`. This index is maintained manually.

## Modules

| Module | Header | Description |
| --- | --- | --- |
| [bitops](<bitops.md>) | `bitops.h` | Single-bit and bit-range operations on 8-bit values, including get, set, clear, toggle, swap, reverse, rotate, population count, and power-of-two checks. |
| [stream](<stream.md>) | `stream.h` | Safe sequential reading of arbitrary bit counts across byte boundaries, with support for bit skipping and byte alignment. |
| [pattern](<pattern.md>) | `pattern.h` | Bit-level pattern matching and searching in streams, with support for custom masks and wildcards. |
| [status](<status.md>) | `status.h` | Status codes (`bf_status`) returned by BitForge functions. |

## Conventions

### Bit Indexing and Endianness

| Module | Bit Order | Description |
| --- | --- | --- |
| `bitops.h` | **LSB-first** | Bit index `0` represents the least significant bit, while index `7` represents the most significant bit. |
| `stream.h`, `pattern.h` | **MSB-first** | Bits are read and matched from most significant to least significant across buffer boundaries, following standard binary formats and protocol specifications. |

### Error Handling

Every function returns a `bf_status` value.

Always check the returned status against `BF_OK` before accessing any output parameters.

See [status](<status.md>) for the complete list of status codes.

## Regenerating Documentation

To regenerate the API reference, run:

``` bash
make docs
```

This command runs Doxygen and Moxygen and generates the module documentation pages in this directory.

## Additional Resources

- [Contributing Guidelines](<CONTRIBUTING.md>)
- [Security Policy](<SECURITY.md>)
- [License](<../LICENSE>)
