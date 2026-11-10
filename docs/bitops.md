

# Bit operations

Single-bit and bit-range operations on 8-bit values.

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_get`](#bf_get)  | Reads a bit from an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_set`](#bf_set)  | Sets a bit in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_clear`](#bf_clear)  | Clears a bit in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_toggle`](#bf_toggle)  | Toggles a bit in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_count_set_bits`](#bf_count_set_bits)  | Counts the set bits in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_is_power_of_two`](#bf_is_power_of_two)  | Checks whether a value is a power of two. |
| [`bf_status`](status.md#bf_status) | [`bf_reverse_bits`](#bf_reverse_bits)  | Reverses the order of the bits in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_rotate_left`](#bf_rotate_left)  | Rotates an 8-bit value to the left. |
| [`bf_status`](status.md#bf_status) | [`bf_rotate_right`](#bf_rotate_right)  | Rotates an 8-bit value to the right. |
| [`bf_status`](status.md#bf_status) | [`bf_get_bits`](#bf_get_bits)  | Reads a range of bits from an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_set_bits`](#bf_set_bits)  | Sets a range of bits in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_clear_bits`](#bf_clear_bits)  | Clears a range of bits in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_toggle_bits`](#bf_toggle_bits)  | Toggles a range of bits in an 8-bit value. |
| [`bf_status`](status.md#bf_status) | [`bf_swap_bits`](#bf_swap_bits)  | Swaps two bits in an 8-bit value. |

---

### bf_get

```cpp
bf_status bf_get(const uint8_t * value, size_t bit_index, bool * result)
```

Defined in include/bitforge/bitops.h:29

Reads a bit from an 8-bit value.

#### Returns
BF_OK on success, BF_EINVAL if a pointer is NULL or bit_index is outside [0, 7].

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const uint8_t *` | Pointer to the value to read. |
| `bit_index` | `size_t` | Bit index in the range [0, 7], where 0 is the least significant bit. |
| `result` | `bool *` | Pointer to receive the bit value. |

---

### bf_set

```cpp
bf_status bf_set(uint8_t * value, size_t bit_index)
```

Defined in include/bitforge/bitops.h:39

Sets a bit in an 8-bit value.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or bit_index is outside [0, 7].

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `bit_index` | `size_t` | Bit index in the range [0, 7]. |

---

### bf_clear

```cpp
bf_status bf_clear(uint8_t * value, size_t bit_index)
```

Defined in include/bitforge/bitops.h:49

Clears a bit in an 8-bit value.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or bit_index is outside [0, 7].

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `bit_index` | `size_t` | Bit index in the range [0, 7]. |

---

### bf_toggle

```cpp
bf_status bf_toggle(uint8_t * value, size_t bit_index)
```

Defined in include/bitforge/bitops.h:59

Toggles a bit in an 8-bit value.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or bit_index is outside [0, 7].

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `bit_index` | `size_t` | Bit index in the range [0, 7]. |

---

### bf_count_set_bits

```cpp
bf_status bf_count_set_bits(const uint8_t * value, uint8_t * result)
```

Defined in include/bitforge/bitops.h:68

Counts the set bits in an 8-bit value.

#### Returns
BF_OK on success, BF_EINVAL if value or result is NULL.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `const uint8_t *` | Pointer to the value to examine. |
| `result` | `uint8_t *` | Pointer to receive the number of set bits (0–8). |

---

### bf_is_power_of_two

```cpp
bf_status bf_is_power_of_two(uint8_t value, bool * result)
```

Defined in include/bitforge/bitops.h:79

Checks whether a value is a power of two.

Zero is not considered a power of two.

#### Returns
BF_OK on success, BF_EINVAL if result is NULL.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t` | Value to check. |
| `result` | `bool *` | Pointer to receive the result. |

---

### bf_reverse_bits

```cpp
bf_status bf_reverse_bits(uint8_t value, uint8_t * result)
```

Defined in include/bitforge/bitops.h:88

Reverses the order of the bits in an 8-bit value.

#### Returns
BF_OK on success, BF_EINVAL if result is NULL.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t` | Value whose bits are to be reversed. |
| `result` | `uint8_t *` | Pointer to receive the reversed value. |

---

### bf_rotate_left

```cpp
bf_status bf_rotate_left(uint8_t value, unsigned int position, uint8_t * result)
```

Defined in include/bitforge/bitops.h:100

Rotates an 8-bit value to the left.

The rotation count is reduced modulo 8.

#### Returns
BF_OK on success, BF_EINVAL if result is NULL.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t` | Value to rotate. |
| `position` | `unsigned int` | Number of bit positions to rotate. |
| `result` | `uint8_t *` | Pointer to receive the rotated value. |

---

### bf_rotate_right

```cpp
bf_status bf_rotate_right(uint8_t value, unsigned int position, uint8_t * result)
```

Defined in include/bitforge/bitops.h:112

Rotates an 8-bit value to the right.

The rotation count is reduced modulo 8.

#### Returns
BF_OK on success, BF_EINVAL if result is NULL.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t` | Value to rotate. |
| `position` | `unsigned int` | Number of bit positions to rotate. |
| `result` | `uint8_t *` | Pointer to receive the rotated value. |

---

### bf_get_bits

```cpp
bf_status bf_get_bits(uint8_t value, size_t bit_index, size_t bit_count, uint8_t * result)
```

Defined in include/bitforge/bitops.h:128

Reads a range of bits from an 8-bit value.

The extracted bits are shifted to the least significant positions.

#### Returns
BF_OK on success, BF_EINVAL if result is NULL or the requested bit range is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t` | Source 8-bit value. |
| `bit_index` | `size_t` | Index of the first bit, counted from the least significant bit (0). |
| `bit_count` | `size_t` | Number of bits to read (1–8). |
| `result` | `uint8_t *` | Pointer to receive the extracted value. |

---

### bf_set_bits

```cpp
bf_status bf_set_bits(uint8_t * value, size_t bit_index, size_t bit_count)
```

Defined in include/bitforge/bitops.h:144

Sets a range of bits in an 8-bit value.

Sets all bits in the specified range to 1. Bits outside the range remain unchanged.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or the requested bit range is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `bit_index` | `size_t` | Index of the first bit, counted from the least significant bit (0). |
| `bit_count` | `size_t` | Number of bits to set (1–8). |

---

### bf_clear_bits

```cpp
bf_status bf_clear_bits(uint8_t * value, size_t bit_index, size_t bit_count)
```

Defined in include/bitforge/bitops.h:159

Clears a range of bits in an 8-bit value.

Sets all bits in the specified range to 0. Bits outside the range remain unchanged.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or the requested bit range is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `bit_index` | `size_t` | Index of the first bit, counted from the least significant bit (0). |
| `bit_count` | `size_t` | Number of bits to clear (1–8). |

---

### bf_toggle_bits

```cpp
bf_status bf_toggle_bits(uint8_t * value, size_t bit_index, size_t bit_count)
```

Defined in include/bitforge/bitops.h:174

Toggles a range of bits in an 8-bit value.

Inverts all bits in the specified range. Bits outside the range remain unchanged.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or the requested bit range is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `bit_index` | `size_t` | Index of the first bit, counted from the least significant bit (0). |
| `bit_count` | `size_t` | Number of bits to toggle (1–8). |

---

### bf_swap_bits

```cpp
bf_status bf_swap_bits(uint8_t * value, size_t first_bit, size_t second_bit)
```

Defined in include/bitforge/bitops.h:190

Swaps two bits in an 8-bit value.

Exchanges the values of the two specified bits. Other bits remain unchanged.

#### Returns
BF_OK on success, BF_EINVAL if value is NULL or either bit index is outside the range 0–7.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `value` | `uint8_t *` | Pointer to the value to modify. |
| `first_bit` | `size_t` | Index of the first bit, counted from the least significant bit (0). |
| `second_bit` | `size_t` | Index of the second bit, counted from the least significant bit (0). |

