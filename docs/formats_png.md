

# PNG Format Parser

Zero-allocation C17 parser for PNG signatures, `IHDR` headers, chunk streams, and CRC-32 verification.

Parses the 8-byte PNG signature, validates `IHDR`, iterates over PNG chunks (`length`, `type`, zero-copy `data` slice, big-endian `crc`), and computes or verifies ISO 3309 / IEEE 802.3 CRC-32 checksums over [`bf_stream_t`](stream.md#bf_stream_t).

## Classes

| Name | Description |
|------|-------------|
| [`bf_png_ihdr_t`](#bf_png_ihdr_t) | Parsed PNG `IHDR` header fields. |
| [`bf_png_chunk_t`](#bf_png_chunk_t) | Zero-copy view of a single PNG chunk inside the stream buffer. |

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_png_read_signature`](#bf_png_read_signature)  | Verifies and consumes the 8-byte PNG signature at current stream offset. |
| [`bf_status`](status.md#bf_status) | [`bf_png_read_ihdr`](#bf_png_read_ihdr)  | Reads and validates the PNG `IHDR` chunk (length, type, payload, CRC). |
| [`bf_status`](status.md#bf_status) | [`bf_png_read_chunk`](#bf_png_read_chunk)  | Reads the next PNG chunk from the stream. |
| `uint32_t` | [`bf_png_crc32`](#bf_png_crc32)  | Computes standard PNG CRC-32 (ISO 3309 / IEEE 802.3) over a byte slice. |
| [`bf_status`](status.md#bf_status) | [`bf_png_verify_chunk_crc`](#bf_png_verify_chunk_crc)  | Verifies chunk CRC-32 against chunk `type` (4 bytes) and payload `data`. |
| `bool` | [`bf_png_chunk_is`](#bf_png_chunk_is)  | Checks whether `chunk->type` matches the 4-character `four_cc` tag. |

---

### bf_png_read_signature

```cpp
bf_status bf_png_read_signature(bf_stream_t * stream)
```

Defined in include/bitforge/formats/png.h:75

Verifies and consumes the 8-byte PNG signature at current stream offset.

Leaves [`Streams`](stream.md#streams) position unchanged on mismatch ([`BF_EFORMAT`](status.md#bf_status)).

#### Returns
[`BF_OK`](status.md#bf_status) on match, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at PNG signature. |

---

### bf_png_read_ihdr

```cpp
bf_status bf_png_read_ihdr(bf_stream_t * stream, bf_png_ihdr_t * out_ihdr)
```

Defined in include/bitforge/formats/png.h:88

Reads and validates the PNG `IHDR` chunk (length, type, payload, CRC).

Expects [`Streams`](stream.md#streams) positioned right after the 8-byte PNG signature (or at `IHDR`), and advances past `IHDR` CRC on success.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at `IHDR` chunk. |
| `out_ihdr` | [`bf_png_ihdr_t`](#bf_png_ihdr_t) * | Populated `IHDR` header on success. |

---

### bf_png_read_chunk

```cpp
bf_status bf_png_read_chunk(bf_stream_t * stream, bf_png_chunk_t * out_chunk)
```

Defined in include/bitforge/formats/png.h:102

Reads the next PNG chunk from the stream.

Advances [`Streams`](stream.md#streams) past `length` (4 B), `type` (4 B), `data` (`length` B), and `crc` (4 B); returns [`BF_EOF`](status.md#bf_status) at exact end of stream with 0 remaining bytes.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EOF`](status.md#bf_status), [`BF_EINVAL`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at a PNG chunk. |
| `out_chunk` | [`bf_png_chunk_t`](#bf_png_chunk_t) * | Populated chunk view on success. |

---

### bf_png_crc32

```cpp
uint32_t bf_png_crc32(const uint8_t * data, size_t length)
```

Defined in include/bitforge/formats/png.h:112

Computes standard PNG CRC-32 (ISO 3309 / IEEE 802.3) over a byte slice.

#### Returns
Computed 32-bit CRC checksum.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `data` | `const uint8_t *` | Input byte buffer (may be `NULL` only when `length == 0`). |
| `length` | `size_t` | Number of bytes to hash. |

---

### bf_png_verify_chunk_crc

```cpp
bf_status bf_png_verify_chunk_crc(const bf_png_chunk_t * chunk)
```

Defined in include/bitforge/formats/png.h:122

Verifies chunk CRC-32 against chunk `type` (4 bytes) and payload `data`.

#### Returns
[`BF_OK`](status.md#bf_status) if matching, [`BF_EFORMAT`](status.md#bf_status) on mismatch, or [`BF_EINVAL`](status.md#bf_status) on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `chunk` | const [`bf_png_chunk_t`](#bf_png_chunk_t) * | Chunk view to verify. |

---

### bf_png_chunk_is

```cpp
bool bf_png_chunk_is(const bf_png_chunk_t * chunk, const char four_cc)
```

Defined in include/bitforge/formats/png.h:133

Checks whether `chunk->type` matches the 4-character `four_cc` tag.

#### Returns
`true` if `chunk` and `four_cc` are valid and match, `false` otherwise.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `chunk` | const [`bf_png_chunk_t`](#bf_png_chunk_t) * | Chunk view to inspect. |
| `four_cc` | `const char` | 4-character chunk type code (e.g., `"IHDR"`, `"IDAT"`, `"IEND"`). |


## Class Definitions



### bf_png_ihdr_t

```cpp
#include <png.h>
```

```cpp
struct bf_png_ihdr_t
```

Defined in include/bitforge/formats/png.h:42

Parsed PNG `IHDR` header fields.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`width`](#width-1)  | Image width in pixels (`> 0`). |
| `uint32_t` | [`height`](#height-1)  | Image height in pixels (`> 0`). |
| `uint8_t` | [`bit_depth`](#bit_depth)  | Bits per sample or palette index. |
| `uint8_t` | [`color_type`](#color_type)  | PNG color type (`0`, `2`, `3`, `4`, `6`). |
| `uint8_t` | [`compression`](#compression-1)  | Compression method (`0` = deflate/inflate). |
| `uint8_t` | [`filter`](#filter)  | Filter method (`0` = adaptive filtering). |
| `uint8_t` | [`interlace`](#interlace)  | Interlace method (`0` = none, `1` = Adam7). |

---

##### width

```cpp
uint32_t width
```

Defined in include/bitforge/formats/png.h:43

Image width in pixels (`> 0`).

---

##### height

```cpp
uint32_t height
```

Defined in include/bitforge/formats/png.h:44

Image height in pixels (`> 0`).

---

##### bit_depth

```cpp
uint8_t bit_depth
```

Defined in include/bitforge/formats/png.h:45

Bits per sample or palette index.

---

##### color_type

```cpp
uint8_t color_type
```

Defined in include/bitforge/formats/png.h:46

PNG color type (`0`, `2`, `3`, `4`, `6`).

---

##### compression

```cpp
uint8_t compression
```

Defined in include/bitforge/formats/png.h:47

Compression method (`0` = deflate/inflate).

---

##### filter

```cpp
uint8_t filter
```

Defined in include/bitforge/formats/png.h:48

Filter method (`0` = adaptive filtering).

---

##### interlace

```cpp
uint8_t interlace
```

Defined in include/bitforge/formats/png.h:49

Interlace method (`0` = none, `1` = Adam7).



### bf_png_chunk_t

```cpp
#include <png.h>
```

```cpp
struct bf_png_chunk_t
```

Defined in include/bitforge/formats/png.h:58

Zero-copy view of a single PNG chunk inside the stream buffer.

The [`data`](#data-7) pointer references bytes directly inside `stream->data` without allocating memory and remains valid while the underlying buffer stays alive.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`length`](#length)  | Length of chunk data payload in bytes. |
| `char` | [`type`](#type-3)  | 4-char ASCII chunk type + NUL terminator. |
| `const uint8_t *` | [`data`](#data-7)  | Pointer to payload bytes (`NULL` if `length == 0`). |
| `uint32_t` | [`crc`](#crc)  | Big-endian CRC-32 stored at end of chunk. |

---

##### length

```cpp
uint32_t length
```

Defined in include/bitforge/formats/png.h:59

Length of chunk data payload in bytes.

---

##### type

```cpp
char type[5]
```

Defined in include/bitforge/formats/png.h:60

4-char ASCII chunk type + NUL terminator.

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/formats/png.h:62

Pointer to payload bytes (`NULL` if `length == 0`).

---

##### crc

```cpp
uint32_t crc
```

Defined in include/bitforge/formats/png.h:63

Big-endian CRC-32 stored at end of chunk.

