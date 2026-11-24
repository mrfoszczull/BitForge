

# Patterns

Bit-level pattern matching and searching with wildcard support.

## Classes

| Name | Description |
|------|-------------|
| [`bf_pattern_t`](#bf_pattern_t) | A bit pattern with a mask. |

## Macros

| Name | Description |
|------|-------------|
| [`BF_MAX_PATTERN_BITS`](#bf_max_pattern_bits)  | Maximum number of bits in a pattern. |

---

### BF_MAX_PATTERN_BITS

```cpp
#define BF_MAX_PATTERN_BITS 64
```

Defined in include/bitforge/pattern.h:21

Maximum number of bits in a pattern.

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_pattern_init`](#bf_pattern_init)  | Initializes a standard bit pattern (all bits compared) from a byte buffer. |
| [`bf_status`](status.md#bf_status) | [`bf_pattern_init_masked`](#bf_pattern_init_masked)  | Initializes a bit pattern with a custom bitmask (supporting wildcards). |
| [`bf_status`](status.md#bf_status) | [`bf_pattern_parse_hex`](#bf_pattern_parse_hex)  | Parses a hexadecimal pattern string with optional "??" byte wildcards. |
| [`bf_status`](status.md#bf_status) | [`bf_pattern_match`](#bf_pattern_match)  | Checks whether the pattern matches at the current stream position. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_find_pattern`](#bf_stream_find_pattern)  | Searches the stream forward from its current position for a pattern. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_find_pattern_from`](#bf_stream_find_pattern_from)  | Searches for a pattern starting at a given byte and bit offset. |
| [`bf_status`](status.md#bf_status) | [`bf_stream_find_pattern_all`](#bf_stream_find_pattern_all)  | Finds all occurrences of a pattern from the current stream position. |

---

### bf_pattern_init

```cpp
bf_status bf_pattern_init(bf_pattern_t * pattern, const uint8_t * data, size_t bit_count)
```

Defined in include/bitforge/pattern.h:46

Initializes a standard bit pattern (all bits compared) from a byte buffer.

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | [`bf_pattern_t`](#bf_pattern_t) * | Pointer to the pattern structure to initialize. |
| `data` | `const uint8_t *` | Buffer containing the pattern data (MSB-first). |
| `bit_count` | `size_t` | Number of bits to read from data (1 to BF_MAX_PATTERN_BITS). |

---

### bf_pattern_init_masked

```cpp
bf_status bf_pattern_init_masked(bf_pattern_t * pattern, const uint8_t * data, const uint8_t * mask, size_t bit_count)
```

Defined in include/bitforge/pattern.h:61

Initializes a bit pattern with a custom bitmask (supporting wildcards).

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | [`bf_pattern_t`](#bf_pattern_t) * | Pointer to the pattern structure to initialize. |
| `data` | `const uint8_t *` | Buffer containing the pattern data (MSB-first). |
| `mask` | `const uint8_t *` | Buffer containing the bitmask (MSB-first; 1 = compare, 0 = wildcard). |
| `bit_count` | `size_t` | Number of bits to read from buffers (1 to BF_MAX_PATTERN_BITS). |

---

### bf_pattern_parse_hex

```cpp
bf_status bf_pattern_parse_hex(bf_pattern_t * pattern, const char * text)
```

Defined in include/bitforge/pattern.h:73

Parses a hexadecimal pattern string with optional "??" byte wildcards.

Example: "48 8B ?? ?? 89 45 ??" (up to 8 bytes / 64 bits).

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments or syntax.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | [`bf_pattern_t`](#bf_pattern_t) * | Pointer to the pattern structure to initialize. |
| `text` | `const char *` | Null-terminated hex string. |

---

### bf_pattern_match

```cpp
bf_status bf_pattern_match(const bf_pattern_t * pattern, const bf_stream_t * stream, bool * out_match)
```

Defined in include/bitforge/pattern.h:86

Checks whether the pattern matches at the current stream position.

Does not modify the stream position.

#### Returns
BF_OK on success (including when out_match is set to false), or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | const [`bf_pattern_t`](#bf_pattern_t) * | Pattern to match. |
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Bit stream to inspect. |
| `out_match` | `bool *` | Output flag set to true on match, false otherwise. |

---

### bf_stream_find_pattern

```cpp
bf_status bf_stream_find_pattern(bf_stream_t * stream, const bf_pattern_t * pattern)
```

Defined in include/bitforge/pattern.h:100

Searches the stream forward from its current position for a pattern.

If found, stream is positioned at the start of the first match. If not found, stream position is left at the end of the search.

#### Returns
BF_OK if found, BF_ENOTFOUND if not found, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Bit stream to search. |
| `pattern` | const [`bf_pattern_t`](#bf_pattern_t) * | Pattern to find. |

---

### bf_stream_find_pattern_from

```cpp
bf_status bf_stream_find_pattern_from(const bf_stream_t * stream, const bf_pattern_t * pattern, size_t start_byte, uint8_t start_bit, size_t * out_byte, uint8_t * out_bit)
```

Defined in include/bitforge/pattern.h:117

Searches for a pattern starting at a given byte and bit offset.

Does not modify the caller's stream position.

#### Returns
BF_OK if found, BF_ENOTFOUND if not found, or BF_EINVAL on invalid arguments or start position.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Bit stream to search. |
| `pattern` | const [`bf_pattern_t`](#bf_pattern_t) * | Pattern to find. |
| `start_byte` | `size_t` | Starting byte offset in stream. |
| `start_bit` | `uint8_t` | Starting bit offset within start_byte (0-7). |
| `out_byte` | `size_t *` | Output byte offset of the first match. |
| `out_bit` | `uint8_t *` | Output bit offset within out_byte of the first match. |

---

### bf_stream_find_pattern_all

```cpp
bf_status bf_stream_find_pattern_all(const bf_stream_t * stream, const bf_pattern_t * pattern, size_t * positions, size_t capacity, size_t * out_count)
```

Defined in include/bitforge/pattern.h:136

Finds all occurrences of a pattern from the current stream position.

Does not modify the caller's stream position. Positions are recorded as bit offsets from the beginning of the underlying data buffer. Up to capacity positions are written to positions, while *out_count receives the total number of matches found.

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | const [`bf_stream_t`](stream.md#bf_stream_t) * | Bit stream to search. |
| `pattern` | const [`bf_pattern_t`](#bf_pattern_t) * | Pattern to find. |
| `positions` | `size_t *` | Output array of bit offsets (may be NULL if capacity == 0). |
| `capacity` | `size_t` | Maximum number of entries that can be stored in positions. |
| `out_count` | `size_t *` | Receives total number of matches found. |


## Class Definitions



### bf_pattern_t

```cpp
#include <pattern.h>
```

```cpp
struct bf_pattern_t
```

Defined in include/bitforge/pattern.h:30

A bit pattern with a mask.

Bits are stored in MSB-first read order across the least significant bit_count bits of data and mask (bits >= bit_count are zeroed). A bit with mask bit == 1 must match data; mask bit == 0 is a wildcard.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`data`](#data-3)  | Expected bit values (LSB-aligned window). |
| `uint64_t` | [`mask`](#mask)  | Comparison mask (1 = compare, 0 = wildcard). |
| `size_t` | [`bit_count`](#bit_count)  | Number of valid bits (1 to BF_MAX_PATTERN_BITS). |

---

##### data

```cpp
uint64_t data
```

Defined in include/bitforge/pattern.h:31

Expected bit values (LSB-aligned window).

---

##### mask

```cpp
uint64_t mask
```

Defined in include/bitforge/pattern.h:32

Comparison mask (1 = compare, 0 = wildcard).

---

##### bit_count

```cpp
size_t bit_count
```

Defined in include/bitforge/pattern.h:34

Number of valid bits (1 to BF_MAX_PATTERN_BITS).

