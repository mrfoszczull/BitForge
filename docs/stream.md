

# Streams

Safe sequential bit stream processing and parsing.

## Classes

| Name | Description |
|------|-------------|
| [`bf_stream_t`](#bf_stream_t) | Represents a bit-level memory stream for binary analysis. |

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_stream_init`](#bf_stream_init)  | Initialize a bit stream with a given data buffer. |
| `bool` | [`bf_stream_has_remaining`](#bf_stream_has_remaining)  | Check if the stream has at least a specified number of bits remaining. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_bits`](#bf_stream_read_bits)  | Read a specified number of bits from the stream. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_skip_bits`](#bf_stream_skip_bits)  | Skip a specified number of bits in the stream. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_align`](#bf_stream_align)  | Align the stream to the next byte boundary. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_byte`](#bf_stream_read_byte)  | Read a single full byte (8 bits) from the stream. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_bool`](#bf_stream_read_bool)  | Read a single bit from the stream as a boolean value. |

---

### bf_stream_init

```cpp
bf_status bf_stream_init(bf_stream_t * stream, const uint8_t * data, size_t size)
```

Defined in include/bitforge/stream.h:37

Initialize a bit stream with a given data buffer.

#### Returns
[bf_status](status.md#bf_status) BF_OK on success, or BF_EINVAL if arguments are invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the stream structure to initialize. |
| `data` | `const uint8_t *` | Pointer to the source data buffer. |
| `size` | `size_t` | Size of the data buffer in bytes. |

---

### bf_stream_has_remaining

```cpp
bool bf_stream_has_remaining(const bf_stream_t * stream, size_t bit_count)
```

Defined in include/bitforge/stream.h:46

Check if the stream has at least a specified number of bits remaining.

#### Returns
true if enough bits remain, false otherwise.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `bit_count` | `size_t` | Number of bits to check. |

---

### bf_stream_read_bits

```cpp
bf_status bf_stream_read_bits(bf_stream_t * stream, size_t bit_count, uint32_t * out_value)
```

Defined in include/bitforge/stream.h:56

Read a specified number of bits from the stream.

#### Returns
[bf_status](status.md#bf_status) BF_OK on success, or an error code on failure.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `bit_count` | `size_t` | Number of bits to read (1 to 32). |
| `out_value` | `uint32_t *` | Pointer to store the read value. |

---

### bf_stream_skip_bits

```cpp
bf_status bf_stream_skip_bits(bf_stream_t * stream, size_t bit_count)
```

Defined in include/bitforge/stream.h:66

Skip a specified number of bits in the stream.

#### Returns
[bf_status](status.md#bf_status) BF_OK on success, or an error code on failure.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `bit_count` | `size_t` | Number of bits to skip. |

---

### bf_stream_align

```cpp
bf_status bf_stream_align(bf_stream_t * stream)
```

Defined in include/bitforge/stream.h:77

Align the stream to the next byte boundary.

If the stream is not currently at a byte boundary (bit_offset != 0), it advances to the beginning of the next byte.

#### Returns
[bf_status](status.md#bf_status) BF_OK on success, or an error code on failure.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |

---

### bf_stream_read_byte

```cpp
bf_status bf_stream_read_byte(bf_stream_t * stream, uint8_t * out_value)
```

Defined in include/bitforge/stream.h:86

Read a single full byte (8 bits) from the stream.

#### Returns
[bf_status](status.md#bf_status) BF_OK on success, or an error code on failure.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `out_value` | `uint8_t *` | Pointer to store the read byte. |

---

### bf_stream_read_bool

```cpp
bf_status bf_stream_read_bool(bf_stream_t * stream, bool * out_value)
```

Defined in include/bitforge/stream.h:95

Read a single bit from the stream as a boolean value.

#### Returns
[bf_status](status.md#bf_status) BF_OK on success, or an error code on failure.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `out_value` | `bool *` | Pointer to store the boolean result. |


## Class Definitions



### bf_stream_t

```cpp
#include <stream.h>
```

```cpp
struct bf_stream_t
```

Defined in include/bitforge/stream.h:22

Represents a bit-level memory stream for binary analysis.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`data`](#data)  | Pointer to the underlying data buffer |
| `size_t` | [`size`](#size)  | Total size of the buffer in bytes |
| `size_t` | [`byte_offset`](#byte_offset)  | Current byte index |
| `uint8_t` | [`bit_offset`](#bit_offset)  | Current bit index within the byte (0-7) |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/stream.h:23

Pointer to the underlying data buffer

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/stream.h:24

Total size of the buffer in bytes

---

##### byte_offset

```cpp
size_t byte_offset
```

Defined in include/bitforge/stream.h:25

Current byte index

---

##### bit_offset

```cpp
uint8_t bit_offset
```

Defined in include/bitforge/stream.h:26

Current bit index within the byte (0-7)

