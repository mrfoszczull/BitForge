

# Streams

Sequential MSB-first bit and byte stream over a memory buffer.

## Classes

| Name | Description |
|------|-------------|
| [`bf_stream_t`](#bf_stream_t) | Represents a bit-level memory stream for binary analysis. |

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_stream_init`](#bf_stream_init)  | Initializes a bit stream with a given data buffer. |
| `bool` | [`bf_stream_has_remaining`](#bf_stream_has_remaining)  | Checks if the stream has at least a specified number of bits remaining. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_bits`](#bf_stream_read_bits)  | Reads 1 to 32 bits from the stream in MSB-first order. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_byte`](#bf_stream_read_byte)  | Reads a single full byte (8 bits) in MSB-first order. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_bytes`](#bf_stream_read_bytes)  | Reads multiple bytes from a byte-aligned stream. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_read_bool`](#bf_stream_read_bool)  | Reads a single bit as a boolean value (MSB-first). |
| [`bf_status`](status.md#bf_status) | [`bf_stream_skip_bits`](#bf_stream_skip_bits)  | Advances the stream position by bit_count bits. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_align`](#bf_stream_align)  | Aligns the stream to the next byte boundary. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_seek`](#bf_stream_seek)  | Sets the stream position to a specific byte and bit offset. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_tell`](#bf_stream_tell)  | Gets the current byte and bit offset of the stream. |

---

### bf_stream_init

```cpp
bf_status bf_stream_init(bf_stream_t * stream, const uint8_t * data, size_t size)
```

Defined in include/bitforge/stream.h:39

Initializes a bit stream with a given data buffer.

#### Returns
BF_OK on success, or BF_EINVAL if arguments are invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the stream structure to initialize. |
| `data` | `const uint8_t *` | Pointer to the source data buffer (may be NULL only when size == 0). |
| `size` | `size_t` | Size of the data buffer in bytes. |

---

### bf_stream_has_remaining

```cpp
bool bf_stream_has_remaining(const bf_stream_t * stream, size_t bit_count)
```

Defined in include/bitforge/stream.h:50

Checks if the stream has at least a specified number of bits remaining.

#### Returns
true if the stream is valid and at least bit_count bits remain, false otherwise.

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

Defined in include/bitforge/stream.h:64

Reads 1 to 32 bits from the stream in MSB-first order.

Extracted bits are placed in the least significant bit_count bits of *out_value. Stream position is unchanged on error.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if fewer than bit_count bits remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `bit_count` | `size_t` | Number of bits to read (1 to 32). |
| `out_value` | `uint32_t *` | Pointer to store the read value. |

---

### bf_stream_read_byte

```cpp
bf_status bf_stream_read_byte(bf_stream_t * stream, uint8_t * out_value)
```

Defined in include/bitforge/stream.h:75

Reads a single full byte (8 bits) in MSB-first order.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if fewer than 8 bits remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `out_value` | `uint8_t *` | Pointer to store the read byte. |

---

### bf_stream_read_bytes

```cpp
bf_status bf_stream_read_bytes(bf_stream_t * stream, uint8_t * destination, size_t length)
```

Defined in include/bitforge/stream.h:88

Reads multiple bytes from a byte-aligned stream.

Requires stream->bit_offset == 0.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or unaligned stream, or BF_EOF if fewer than length bytes remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `destination` | `uint8_t *` | Destination buffer (may be NULL only when length == 0). |
| `length` | `size_t` | Number of bytes to read. |

---

### bf_stream_read_bool

```cpp
bf_status bf_stream_read_bool(bf_stream_t * stream, bool * out_value)
```

Defined in include/bitforge/stream.h:99

Reads a single bit as a boolean value (MSB-first).

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if no bits remain.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `out_value` | `bool *` | Pointer to store the boolean result. |

---

### bf_stream_skip_bits

```cpp
bf_status bf_stream_skip_bits(bf_stream_t * stream, size_t bit_count)
```

Defined in include/bitforge/stream.h:111

Advances the stream position by bit_count bits.

Position is unchanged if fewer than bit_count bits remain.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if fewer than bit_count bits remain.

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

Defined in include/bitforge/stream.h:121

Aligns the stream to the next byte boundary.

If bit_offset != 0, advances to bit_offset == 0 at the next byte.

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments or stream state.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |

---

### bf_stream_seek

```cpp
bf_status bf_stream_seek(bf_stream_t * stream, size_t byte_offset, uint8_t bit_offset)
```

Defined in include/bitforge/stream.h:132

Sets the stream position to a specific byte and bit offset.

#### Returns
BF_OK on success, BF_EINVAL if position is invalid or out of bounds.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `byte_offset` | `size_t` | Target byte offset in [0, stream->size]. |
| `bit_offset` | `uint8_t` | Target bit offset in [0, 7] (must be 0 when byte_offset == stream->size). |

---

### bf_stream_tell

```cpp
bf_status bf_stream_tell(const bf_stream_t * stream, size_t * out_byte_offset, uint8_t * out_bit_offset)
```

Defined in include/bitforge/stream.h:143

Gets the current byte and bit offset of the stream.

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](#bf_stream_t) * | Pointer to the bit stream. |
| `out_byte_offset` | `size_t *` | Optional pointer to receive current byte offset. |
| `out_bit_offset` | `uint8_t *` | Optional pointer to receive current bit offset (0-7). |


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
| `const uint8_t *` | [`data`](#data-2)  | Pointer to the underlying data buffer. |
| `size_t` | [`size`](#size-2)  | Total size of the buffer in bytes. |
| `size_t` | [`byte_offset`](#byte_offset-1)  | Current byte index in [0, size]. |
| `uint8_t` | [`bit_offset`](#bit_offset-1)  | Current bit index within byte (0-7, MSB-first). |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/stream.h:23

Pointer to the underlying data buffer.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/stream.h:24

Total size of the buffer in bytes.

---

##### byte_offset

```cpp
size_t byte_offset
```

Defined in include/bitforge/stream.h:25

Current byte index in [0, size].

---

##### bit_offset

```cpp
uint8_t bit_offset
```

Defined in include/bitforge/stream.h:27

Current bit index within byte (0-7, MSB-first).

