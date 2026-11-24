

# Generic Parser

Endian-aware integer, bitfield, string, float, and validation over streams.

## Classes

| Name | Description |
|------|-------------|
| [`bf_parser_diag_t`](#bf_parser_diag_t) | Diagnostic context capturing stream offset and error details. |

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_endian_t`](#bf_endian_t)  | Byte order (endianness) for multi-byte values. |

---

### bf_endian_t

```cpp
enum bf_endian_t
```

Defined in include/bitforge/parser.h:24

Byte order (endianness) for multi-byte values.

| Value | Description |
|-------|-------------|
| `BF_ENDIAN_LITTLE` | Least significant byte first. |
| `BF_ENDIAN_BIG` | Most significant byte first. |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u8`](#bf_parser_read_u8)  | Reads an unsigned 8-bit integer from a byte-aligned stream. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u16_le`](#bf_parser_read_u16_le)  | Reads an unsigned 16-bit little-endian integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u16_be`](#bf_parser_read_u16_be)  | Reads an unsigned 16-bit big-endian integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u24_le`](#bf_parser_read_u24_le)  | Reads an unsigned 24-bit little-endian integer into `uint32_t`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u24_be`](#bf_parser_read_u24_be)  | Reads an unsigned 24-bit big-endian integer into `uint32_t`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u32_le`](#bf_parser_read_u32_le)  | Reads an unsigned 32-bit little-endian integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u32_be`](#bf_parser_read_u32_be)  | Reads an unsigned 32-bit big-endian integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u64_le`](#bf_parser_read_u64_le)  | Reads an unsigned 64-bit little-endian integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_u64_be`](#bf_parser_read_u64_be)  | Reads an unsigned 64-bit big-endian integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i8`](#bf_parser_read_i8)  | Reads a signed 8-bit two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i16_le`](#bf_parser_read_i16_le)  | Reads a signed 16-bit little-endian two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i16_be`](#bf_parser_read_i16_be)  | Reads a signed 16-bit big-endian two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i24_le`](#bf_parser_read_i24_le)  | Reads a signed 24-bit little-endian two's-complement integer into `int32_t`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i24_be`](#bf_parser_read_i24_be)  | Reads a signed 24-bit big-endian two's-complement integer into `int32_t`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i32_le`](#bf_parser_read_i32_le)  | Reads a signed 32-bit little-endian two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i32_be`](#bf_parser_read_i32_be)  | Reads a signed 32-bit big-endian two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i64_le`](#bf_parser_read_i64_le)  | Reads a signed 64-bit little-endian two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_i64_be`](#bf_parser_read_i64_be)  | Reads a signed 64-bit big-endian two's-complement integer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_f32_le`](#bf_parser_read_f32_le)  | Reads an IEEE 754 single-precision little-endian float. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_f32_be`](#bf_parser_read_f32_be)  | Reads an IEEE 754 single-precision big-endian float. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_f64_le`](#bf_parser_read_f64_le)  | Reads an IEEE 754 double-precision little-endian float. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_f64_be`](#bf_parser_read_f64_be)  | Reads an IEEE 754 double-precision big-endian float. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_fixed_string`](#bf_parser_read_fixed_string)  | Reads a fixed-length byte field into a NUL-terminated string buffer. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_cstr`](#bf_parser_read_cstr)  | Reads a NUL-terminated C string from the stream. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_pascal_u8_string`](#bf_parser_read_pascal_u8_string)  | Reads a length-prefixed ("Pascal") string: 1-byte length followed by that many bytes, stored as a NUL-terminated string in out_buf. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_bits`](#bf_parser_read_bits)  | Reads an unsigned bitfield of `bit_count` bits from the stream. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_read_flag`](#bf_parser_read_flag)  | Reads a single bit as a boolean flag. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_check_reserved_bits`](#bf_parser_check_reserved_bits)  | Reads and verifies that `bit_count` reserved bits are all zero. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_expect_bytes`](#bf_parser_expect_bytes)  | Verifies that the next `length` bytes in [`Streams`](stream.md#streams) match `expected`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_expect_u32_be`](#bf_parser_expect_u32_be)  | Verifies that the next 4 big-endian bytes equal `expected`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_expect_u32_le`](#bf_parser_expect_u32_le)  | Verifies that the next 4 little-endian bytes equal `expected`. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_require_bytes`](#bf_parser_require_bytes)  | Ensures at least `required_bytes` remain unread in [`Streams`](stream.md#streams). |
| `void` | [`bf_parser_diag_reset`](#bf_parser_diag_reset)  | Initializes or resets a diagnostic record. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_diag_set`](#bf_parser_diag_set)  | Populates a diagnostic record from the current stream position. |
| [`bf_status`](status.md#bf_status) | [`bf_parser_diag_format`](#bf_parser_diag_format)  | Formats a human-readable summary into caller-provided buffer. |

---

### bf_parser_read_u8

```cpp
bf_status bf_parser_read_u8(bf_stream_t * stream, uint8_t * out_value)
```

Defined in include/bitforge/parser.h:51

Reads an unsigned 8-bit integer from a byte-aligned stream.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 1 byte remains.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint8_t *` | Destination for the read value. |

---

### bf_parser_read_u16_le

```cpp
bf_status bf_parser_read_u16_le(bf_stream_t * stream, uint16_t * out_value)
```

Defined in include/bitforge/parser.h:61

Reads an unsigned 16-bit little-endian integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 2 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint16_t *` | Destination for the read value. |

---

### bf_parser_read_u16_be

```cpp
bf_status bf_parser_read_u16_be(bf_stream_t * stream, uint16_t * out_value)
```

Defined in include/bitforge/parser.h:71

Reads an unsigned 16-bit big-endian integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 2 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint16_t *` | Destination for the read value. |

---

### bf_parser_read_u24_le

```cpp
bf_status bf_parser_read_u24_le(bf_stream_t * stream, uint32_t * out_value)
```

Defined in include/bitforge/parser.h:81

Reads an unsigned 24-bit little-endian integer into `uint32_t`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 3 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint32_t *` | Destination for the read value. |

---

### bf_parser_read_u24_be

```cpp
bf_status bf_parser_read_u24_be(bf_stream_t * stream, uint32_t * out_value)
```

Defined in include/bitforge/parser.h:91

Reads an unsigned 24-bit big-endian integer into `uint32_t`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 3 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint32_t *` | Destination for the read value. |

---

### bf_parser_read_u32_le

```cpp
bf_status bf_parser_read_u32_le(bf_stream_t * stream, uint32_t * out_value)
```

Defined in include/bitforge/parser.h:101

Reads an unsigned 32-bit little-endian integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint32_t *` | Destination for the read value. |

---

### bf_parser_read_u32_be

```cpp
bf_status bf_parser_read_u32_be(bf_stream_t * stream, uint32_t * out_value)
```

Defined in include/bitforge/parser.h:111

Reads an unsigned 32-bit big-endian integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint32_t *` | Destination for the read value. |

---

### bf_parser_read_u64_le

```cpp
bf_status bf_parser_read_u64_le(bf_stream_t * stream, uint64_t * out_value)
```

Defined in include/bitforge/parser.h:121

Reads an unsigned 64-bit little-endian integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 8 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint64_t *` | Destination for the read value. |

---

### bf_parser_read_u64_be

```cpp
bf_status bf_parser_read_u64_be(bf_stream_t * stream, uint64_t * out_value)
```

Defined in include/bitforge/parser.h:131

Reads an unsigned 64-bit big-endian integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 8 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `uint64_t *` | Destination for the read value. |

---

### bf_parser_read_i8

```cpp
bf_status bf_parser_read_i8(bf_stream_t * stream, int8_t * out_value)
```

Defined in include/bitforge/parser.h:143

Reads a signed 8-bit two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 1 byte remains.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int8_t *` | Destination for the read value. |

---

### bf_parser_read_i16_le

```cpp
bf_status bf_parser_read_i16_le(bf_stream_t * stream, int16_t * out_value)
```

Defined in include/bitforge/parser.h:153

Reads a signed 16-bit little-endian two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 2 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int16_t *` | Destination for the read value. |

---

### bf_parser_read_i16_be

```cpp
bf_status bf_parser_read_i16_be(bf_stream_t * stream, int16_t * out_value)
```

Defined in include/bitforge/parser.h:163

Reads a signed 16-bit big-endian two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 2 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int16_t *` | Destination for the read value. |

---

### bf_parser_read_i24_le

```cpp
bf_status bf_parser_read_i24_le(bf_stream_t * stream, int32_t * out_value)
```

Defined in include/bitforge/parser.h:174

Reads a signed 24-bit little-endian two's-complement integer into `int32_t`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 3 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int32_t *` | Destination for the read value. |

---

### bf_parser_read_i24_be

```cpp
bf_status bf_parser_read_i24_be(bf_stream_t * stream, int32_t * out_value)
```

Defined in include/bitforge/parser.h:185

Reads a signed 24-bit big-endian two's-complement integer into `int32_t`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 3 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int32_t *` | Destination for the read value. |

---

### bf_parser_read_i32_le

```cpp
bf_status bf_parser_read_i32_le(bf_stream_t * stream, int32_t * out_value)
```

Defined in include/bitforge/parser.h:195

Reads a signed 32-bit little-endian two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int32_t *` | Destination for the read value. |

---

### bf_parser_read_i32_be

```cpp
bf_status bf_parser_read_i32_be(bf_stream_t * stream, int32_t * out_value)
```

Defined in include/bitforge/parser.h:205

Reads a signed 32-bit big-endian two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int32_t *` | Destination for the read value. |

---

### bf_parser_read_i64_le

```cpp
bf_status bf_parser_read_i64_le(bf_stream_t * stream, int64_t * out_value)
```

Defined in include/bitforge/parser.h:215

Reads a signed 64-bit little-endian two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 8 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int64_t *` | Destination for the read value. |

---

### bf_parser_read_i64_be

```cpp
bf_status bf_parser_read_i64_be(bf_stream_t * stream, int64_t * out_value)
```

Defined in include/bitforge/parser.h:225

Reads a signed 64-bit big-endian two's-complement integer.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 8 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `int64_t *` | Destination for the read value. |

---

### bf_parser_read_f32_le

```cpp
bf_status bf_parser_read_f32_le(bf_stream_t * stream, float * out_value)
```

Defined in include/bitforge/parser.h:237

Reads an IEEE 754 single-precision little-endian float.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `float *` | Destination for the read value. |

---

### bf_parser_read_f32_be

```cpp
bf_status bf_parser_read_f32_be(bf_stream_t * stream, float * out_value)
```

Defined in include/bitforge/parser.h:247

Reads an IEEE 754 single-precision big-endian float.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 4 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `float *` | Destination for the read value. |

---

### bf_parser_read_f64_le

```cpp
bf_status bf_parser_read_f64_le(bf_stream_t * stream, double * out_value)
```

Defined in include/bitforge/parser.h:257

Reads an IEEE 754 double-precision little-endian float.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 8 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `double *` | Destination for the read value. |

---

### bf_parser_read_f64_be

```cpp
bf_status bf_parser_read_f64_be(bf_stream_t * stream, double * out_value)
```

Defined in include/bitforge/parser.h:267

Reads an IEEE 754 double-precision big-endian float.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if fewer than 8 bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_value` | `double *` | Destination for the read value. |

---

### bf_parser_read_fixed_string

```cpp
bf_status bf_parser_read_fixed_string(bf_stream_t * stream, size_t length, char * out_buf, size_t buf_size)
```

Defined in include/bitforge/parser.h:283

Reads a fixed-length byte field into a NUL-terminated string buffer.

Requires byte alignment (bit_offset == 0) and buf_size > length. Stream advances by length bytes on success.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EOVERFLOW`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `length` | `size_t` | Number of bytes to read from the stream. |
| `out_buf` | `char *` | Destination string buffer. |
| `buf_size` | `size_t` | Capacity of `out_buf` in bytes. |

---

### bf_parser_read_cstr

```cpp
bf_status bf_parser_read_cstr(bf_stream_t * stream, char * out_buf, size_t buf_size)
```

Defined in include/bitforge/parser.h:299

Reads a NUL-terminated C string from the stream.

Requires byte alignment (bit_offset == 0). Consumes bytes up to and including the terminating '\0'. Returns BF_EFORMAT if no '\0' is found before EOF, or BF_EOVERFLOW if the string (including '\0') does not fit in out_buf.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EFORMAT`](status.md#bf_status), or [`BF_EOVERFLOW`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_buf` | `char *` | Destination string buffer. |
| `buf_size` | `size_t` | Capacity of `out_buf` in bytes. |

---

### bf_parser_read_pascal_u8_string

```cpp
bf_status bf_parser_read_pascal_u8_string(bf_stream_t * stream, char * out_buf, size_t buf_size)
```

Defined in include/bitforge/parser.h:314

Reads a length-prefixed ("Pascal") string: 1-byte length followed by that many bytes, stored as a NUL-terminated string in out_buf.

Requires byte alignment (bit_offset == 0). Returns BF_EOVERFLOW if prefixed length + 1 > buf_size.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EOVERFLOW`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `out_buf` | `char *` | Destination string buffer. |
| `buf_size` | `size_t` | Capacity of `out_buf` in bytes. |

---

### bf_parser_read_bits

```cpp
bf_status bf_parser_read_bits(bf_stream_t * stream, size_t bit_count, uint32_t * out_value)
```

Defined in include/bitforge/parser.h:328

Reads an unsigned bitfield of `bit_count` bits from the stream.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments/width, or [`BF_EOF`](status.md#bf_status) if fewer than `bit_count` bits remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Input stream. |
| `bit_count` | `size_t` | Number of bits to read (`1` to `32`). |
| `out_value` | `uint32_t *` | Destination for the read bitfield value. |

---

### bf_parser_read_flag

```cpp
bf_status bf_parser_read_flag(bf_stream_t * stream, bool * out_flag)
```

Defined in include/bitforge/parser.h:340

Reads a single bit as a boolean flag.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) if no bits remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Input stream. |
| `out_flag` | `bool *` | Destination set to `true` for bit `1` or `false` for bit `0`. |

---

### bf_parser_check_reserved_bits

```cpp
bf_status bf_parser_check_reserved_bits(bf_stream_t * stream, size_t bit_count)
```

Defined in include/bitforge/parser.h:350

Reads and verifies that `bit_count` reserved bits are all zero.

#### Returns
[`BF_OK`](status.md#bf_status) if all bits are `0`, [`BF_EFORMAT`](status.md#bf_status) if any bit is non-zero, [`BF_EINVAL`](status.md#bf_status) on invalid arguments, or [`BF_EOF`](status.md#bf_status) on truncated input.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Input stream. |
| `bit_count` | `size_t` | Number of reserved bits to inspect. |

---

### bf_parser_expect_bytes

```cpp
bf_status bf_parser_expect_bytes(bf_stream_t * stream, const uint8_t * expected, size_t length)
```

Defined in include/bitforge/parser.h:362

Verifies that the next `length` bytes in [`Streams`](stream.md#streams) match `expected`.

#### Returns
[`BF_OK`](status.md#bf_status) on match, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `expected` | `const uint8_t *` | Expected byte sequence. |
| `length` | `size_t` | Number of bytes to compare. |

---

### bf_parser_expect_u32_be

```cpp
bf_status bf_parser_expect_u32_be(bf_stream_t * stream, uint32_t expected)
```

Defined in include/bitforge/parser.h:372

Verifies that the next 4 big-endian bytes equal `expected`.

#### Returns
[`BF_OK`](status.md#bf_status) on match, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `expected` | `uint32_t` | Expected 32-bit big-endian value. |

---

### bf_parser_expect_u32_le

```cpp
bf_status bf_parser_expect_u32_le(bf_stream_t * stream, uint32_t expected)
```

Defined in include/bitforge/parser.h:381

Verifies that the next 4 little-endian bytes equal `expected`.

#### Returns
[`BF_OK`](status.md#bf_status) on match, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned input stream. |
| `expected` | `uint32_t` | Expected 32-bit little-endian value. |

---

### bf_parser_require_bytes

```cpp
bf_status bf_parser_require_bytes(const bf_stream_t * stream, size_t required_bytes)
```

Defined in include/bitforge/parser.h:390

Ensures at least `required_bytes` remain unread in [`Streams`](stream.md#streams).

#### Returns
[`BF_OK`](status.md#bf_status) if available, [`BF_EINVAL`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EOF`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Input stream to check. |
| `required_bytes` | `size_t` | Minimum number of remaining bytes required. |

---

### bf_parser_diag_reset

```cpp
void bf_parser_diag_reset(bf_parser_diag_t * diag)
```

Defined in include/bitforge/parser.h:400

Initializes or resets a diagnostic record.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `diag` | [`bf_parser_diag_t`](#bf_parser_diag_t) * | Diagnostic structure to reset. |

---

### bf_parser_diag_set

```cpp
bf_status bf_parser_diag_set(bf_parser_diag_t * diag, const bf_stream_t * stream, bf_status status, const char * message)
```

Defined in include/bitforge/parser.h:411

Populates a diagnostic record from the current stream position.

#### Returns
[`BF_OK`](status.md#bf_status) on success or [`BF_EINVAL`](status.md#bf_status) on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `diag` | [`bf_parser_diag_t`](#bf_parser_diag_t) * | Diagnostic structure to populate. |
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Optional stream providing current offsets. |
| `status` | [`bf_status`](status.md#bf_status) | Status code to record. |
| `message` | `const char *` | Static diagnostic message string. |

---

### bf_parser_diag_format

```cpp
bf_status bf_parser_diag_format(const bf_parser_diag_t * diag, char * out_buf, size_t buf_size)
```

Defined in include/bitforge/parser.h:422

Formats a human-readable summary into caller-provided buffer.

#### Returns
[`BF_OK`](status.md#bf_status) on success or [`BF_EINVAL`](status.md#bf_status) on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `diag` | const [`bf_parser_diag_t`](#bf_parser_diag_t) * | Diagnostic record to format. |
| `out_buf` | `char *` | Destination buffer. |
| `buf_size` | `size_t` | Capacity of `out_buf` in bytes. |


## Class Definitions



### bf_parser_diag_t

```cpp
#include <parser.h>
```

```cpp
struct bf_parser_diag_t
```

Defined in include/bitforge/parser.h:32

Diagnostic context capturing stream offset and error details.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `size_t` | [`byte_offset`](#byte_offset)  | Byte offset associated with the event/error. |
| `uint8_t` | [`bit_offset`](#bit_offset)  | Bit offset (0-7) associated with the event/error. |
| [`bf_status`](status.md#bf_status) | [`status`](#status)  | Status code recorded. |
| `const char *` | [`message`](#message)  | Static diagnostic message (never allocated). |

---

##### byte_offset

```cpp
size_t byte_offset
```

Defined in include/bitforge/parser.h:33

Byte offset associated with the event/error.

---

##### bit_offset

```cpp
uint8_t bit_offset
```

Defined in include/bitforge/parser.h:34

Bit offset (0-7) associated with the event/error.

---

##### status

```cpp
bf_status status
```

Type: [`bf_status`](status.md#bf_status)

Defined in include/bitforge/parser.h:36

Status code recorded.

---

##### message

```cpp
const char * message
```

Defined in include/bitforge/parser.h:38

Static diagnostic message (never allocated).

