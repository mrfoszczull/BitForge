

# Patterns

Bit-level pattern matching and searching with wildcard support.

## Classes

| Name | Description |
|------|-------------|
| [`bf_pattern_t`](#bf_pattern_t-1) | A bit pattern with a mask. |

## Macros

| Name | Description |
|------|-------------|
| [`BF_MAX_PATTERN_BITS`](#bf_max_pattern_bits)  | Maximum number of bits in a pattern. |

---

### BF_MAX_PATTERN_BITS

```cpp
#define BF_MAX_PATTERN_BITS 64
```

Defined in include/bitforge/pattern.h:20

Maximum number of bits in a pattern.

## Typedefs

| Return | Name | Description |
|--------|------|-------------|
| `struct bf_pattern_t` | [`bf_pattern_t`](#bf_pattern_t)  | A bit pattern with a mask. |

---

### bf_pattern_t

```cpp
using bf_pattern_t = struct bf_pattern_t
```

Defined in include/bitforge/pattern.h:29

A bit pattern with a mask.

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_pattern_init`](#bf_pattern_init)  | Initializes a standard bit pattern from a raw byte buffer. |
| [`bf_status`](status.md#bf_status) | [`bf_pattern_init_masked`](#bf_pattern_init_masked)  | Initializes a bit pattern with a custom bitmask (supporting wildcards). |
| [`bf_status`](status.md#bf_status) | [`bf_stream_find_pattern`](#bf_stream_find_pattern)  | Searches the bit stream for the first occurrence of a pattern. |

---

### bf_pattern_init

```cpp
bf_status bf_pattern_init(bf_pattern_t * pattern, const uint8_t * data, size_t bit_count)
```

Defined in include/bitforge/pattern.h:40

Initializes a standard bit pattern from a raw byte buffer.

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | [`bf_pattern_t`](#bf_pattern_t-1) * | Pointer to the pattern structure to initialize. |
| `data` | `const uint8_t *` | Buffer containing the pattern data. |
| `bit_count` | `size_t` | Number of bits to read from the buffer (max BF_MAX_PATTERN_BITS). |

---

### bf_pattern_init_masked

```cpp
bf_status bf_pattern_init_masked(bf_pattern_t * pattern, const uint8_t * data, const uint8_t * mask, size_t bit_count)
```

Defined in include/bitforge/pattern.h:54

Initializes a bit pattern with a custom bitmask (supporting wildcards).

#### Returns
BF_OK on success, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `pattern` | [`bf_pattern_t`](#bf_pattern_t-1) * | Pointer to the pattern structure to initialize. |
| `data` | `const uint8_t *` | Buffer containing the pattern data. |
| `mask` | `const uint8_t *` | Buffer containing the bitmask (1 = check bit, 0 = wildcard). |
| `bit_count` | `size_t` | Number of bits to read from buffers (max BF_MAX_PATTERN_BITS). |

---

### bf_stream_find_pattern

```cpp
bf_status bf_stream_find_pattern(bf_stream_t * stream, const bf_pattern_t * pattern)
```

Defined in include/bitforge/pattern.h:66

Searches the bit stream for the first occurrence of a pattern.

#### Returns
BF_OK if found (stream is positioned at the start of the match), BF_ENOTFOUND if the pattern is not found, or BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Pointer to the bit stream to search. |
| `pattern` | const [`bf_pattern_t`](#bf_pattern_t-1) * | Pointer to the bit pattern to find. |


## Class Definitions



### bf_pattern_t

```cpp
#include <pattern.h>
```

```cpp
struct bf_pattern_t
```

Defined in include/bitforge/pattern.h:25

A bit pattern with a mask.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint64_t` | [`data`](#data-3)  | Pattern bits. |
| `uint64_t` | [`mask`](#mask)  | Bits that are compared. |
| `size_t` | [`bit_count`](#bit_count)  | Number of valid bits. |

---

##### data

```cpp
uint64_t data
```

Defined in include/bitforge/pattern.h:26

Pattern bits.

---

##### mask

```cpp
uint64_t mask
```

Defined in include/bitforge/pattern.h:27

Bits that are compared.

---

##### bit_count

```cpp
size_t bit_count
```

Defined in include/bitforge/pattern.h:28

Number of valid bits.

