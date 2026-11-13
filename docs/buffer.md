

# Buffers

Memory views and caller-managed buffers.

## Classes

| Name | Description |
|------|-------------|
| [`bf_view_t`](#bf_view_t) | Read-only view of a memory region. |
| [`bf_buffer_t`](#bf_buffer_t) | Mutable buffer backed by caller-provided memory. |

## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_view_init`](#bf_view_init)  | Initializes a read-only memory view. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_init`](#bf_buffer_init)  | Initializes a writable buffer using caller-provided memory. |
| [`bf_status`](status.md#bf_status) | [`bf_view_read`](#bf_view_read)  | Reads bytes from a memory view. |
| [`bf_status`](status.md#bf_status) | [`bf_view_slice`](#bf_view_slice)  | Creates a read-only subview without copying data. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_write`](#bf_buffer_write)  | Writes bytes into a buffer at the specified offset. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_get_view`](#bf_buffer_get_view)  | Creates a read-only view of the buffer's current contents. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_append`](#bf_buffer_append)  | Appends bytes to the end of a buffer. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_clear`](#bf_buffer_clear)  | Clears the logical contents of a buffer. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_fill`](#bf_buffer_fill)  | Fills a range of a buffer with a specified byte value. |
| [`bf_status`](status.md#bf_status) | [`bf_view_compare`](#bf_view_compare)  | Compares the contents of two memory views. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_copy`](#bf_buffer_copy)  | Copies the contents of one buffer into another. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_read`](#bf_buffer_read)  | Reads bytes from a buffer into a destination memory region. |

---

### bf_view_init

```cpp
bf_status bf_view_init(bf_view_t * view, const uint8_t * data, size_t size)
```

Defined in include/bitforge/buffer.h:45

Initializes a read-only memory view.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `view` | [`bf_view_t`](#bf_view_t) * | Destination view. |
| `data` | `const uint8_t *` | Source memory. |
| `size` | `size_t` | Number of bytes available. |

---

### bf_buffer_init

```cpp
bf_status bf_buffer_init(bf_buffer_t * buffer, uint8_t * data, size_t capacity)
```

Defined in include/bitforge/buffer.h:57

Initializes a writable buffer using caller-provided memory.

The buffer starts empty. No memory is allocated.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `data` | `uint8_t *` | Writable memory region. |
| `capacity` | `size_t` | Total available capacity. |

---

### bf_view_read

```cpp
bf_status bf_view_read(const bf_view_t * view, size_t offset, uint8_t * destination, size_t length)
```

Defined in include/bitforge/buffer.h:68

Reads bytes from a memory view.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `view` | const [`bf_view_t`](#bf_view_t) * | Source memory view. |
| `offset` | `size_t` | Starting offset in the view. |
| `destination` | `uint8_t *` | Destination memory. |
| `length` | `size_t` | Number of bytes to read. |

---

### bf_view_slice

```cpp
bf_status bf_view_slice(const bf_view_t * view, size_t offset, size_t length, bf_view_t * result)
```

Defined in include/bitforge/buffer.h:80

Creates a read-only subview without copying data.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `view` | const [`bf_view_t`](#bf_view_t) * | Source memory view. |
| `offset` | `size_t` | Starting offset in the source view. |
| `length` | `size_t` | Number of bytes in the subview. |
| `result` | [`bf_view_t`](#bf_view_t) * | Destination view. |

---

### bf_buffer_write

```cpp
bf_status bf_buffer_write(bf_buffer_t * buffer, size_t offset, const uint8_t * source, size_t length)
```

Defined in include/bitforge/buffer.h:94

Writes bytes into a buffer at the specified offset.

Does not change the buffer's logical size.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `offset` | `size_t` | Starting offset in the buffer. |
| `source` | `const uint8_t *` | Source memory. |
| `length` | `size_t` | Number of bytes to write. |

---

### bf_buffer_get_view

```cpp
bf_status bf_buffer_get_view(const bf_buffer_t * buffer, bf_view_t * view)
```

Defined in include/bitforge/buffer.h:104

Creates a read-only view of the buffer's current contents.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | const [`bf_buffer_t`](#bf_buffer_t) * | Source buffer. |
| `view` | [`bf_view_t`](#bf_view_t) * | Destination view. |

---

### bf_buffer_append

```cpp
bf_status bf_buffer_append(bf_buffer_t * buffer, const uint8_t * source, size_t length)
```

Defined in include/bitforge/buffer.h:118

Appends bytes to the end of a buffer.

Copies data to the buffer starting at its current logical size and increases the size after a successful operation.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or insufficient buffer capacity.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `source` | `const uint8_t *` | Source memory. |
| `length` | `size_t` | Number of bytes to append. |

---

### bf_buffer_clear

```cpp
bf_status bf_buffer_clear(bf_buffer_t * buffer)
```

Defined in include/bitforge/buffer.h:130

Clears the logical contents of a buffer.

Sets the buffer's size to zero without modifying the underlying memory or its capacity.

#### Returns
BF_OK on success, BF_EINVAL if the buffer is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Buffer to clear. |

---

### bf_buffer_fill

```cpp
bf_status bf_buffer_fill(bf_buffer_t * buffer, size_t offset, size_t length, uint8_t value)
```

Defined in include/bitforge/buffer.h:145

Fills a range of a buffer with a specified byte value.

Writes the given value into the specified range without changing the buffer's logical size.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or an out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `offset` | `size_t` | Starting offset in the buffer. |
| `length` | `size_t` | Number of bytes to fill. |
| `value` | `uint8_t` | Byte value used to fill the range. |

---

### bf_view_compare

```cpp
bf_status bf_view_compare(const bf_view_t * a, const bf_view_t * b, bool * equal)
```

Defined in include/bitforge/buffer.h:159

Compares the contents of two memory views.

Compares the sizes and byte contents of both views.

#### Returns
BF_OK on success, BF_EINVAL if any argument is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `a` | const [`bf_view_t`](#bf_view_t) * | First memory view. |
| `b` | const [`bf_view_t`](#bf_view_t) * | Second memory view. |
| `equal` | `bool *` | Output parameter set to true if both views contain identical data, false otherwise. |

---

### bf_buffer_copy

```cpp
bf_status bf_buffer_copy(bf_buffer_t * destination, const bf_buffer_t * source)
```

Defined in include/bitforge/buffer.h:172

Copies the contents of one buffer into another.

Copies all logically used bytes from the source buffer into the destination buffer.

#### Returns
BF_OK on success, BF_EINVAL if an argument is invalid or the destination capacity is insufficient.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `destination` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `source` | const [`bf_buffer_t`](#bf_buffer_t) * | Source buffer. |

---

### bf_buffer_read

```cpp
bf_status bf_buffer_read(const bf_buffer_t * buffer, size_t offset, uint8_t * destination, size_t length)
```

Defined in include/bitforge/buffer.h:184

Reads bytes from a buffer into a destination memory region.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | const [`bf_buffer_t`](#bf_buffer_t) * | Source buffer. |
| `offset` | `size_t` | Starting offset in the buffer (relative to buffer->size). |
| `destination` | `uint8_t *` | Destination memory region. |
| `length` | `size_t` | Number of bytes to read. |


## Class Definitions



### bf_view_t

```cpp
#include <buffer.h>
```

```cpp
struct bf_view_t
```

Defined in include/bitforge/buffer.h:22

Read-only view of a memory region.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`data`](#data)  | Pointer to the first byte of the viewed region. |
| `size_t` | [`size`](#size)  | Number of bytes in the viewed region. |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/buffer.h:24

Pointer to the first byte of the viewed region.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/buffer.h:25

Number of bytes in the viewed region.



### bf_buffer_t

```cpp
#include <buffer.h>
```

```cpp
struct bf_buffer_t
```

Defined in include/bitforge/buffer.h:31

Mutable buffer backed by caller-provided memory.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t *` | [`data`](#data-1)  | Pointer to the caller-provided memory. |
| `size_t` | [`capacity`](#capacity)  | Total available memory in bytes. |
| `size_t` | [`size`](#size-1)  | Number of bytes currently used. |

---

##### data

```cpp
uint8_t * data
```

Defined in include/bitforge/buffer.h:32

Pointer to the caller-provided memory.

---

##### capacity

```cpp
size_t capacity
```

Defined in include/bitforge/buffer.h:33

Total available memory in bytes.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/buffer.h:34

Number of bytes currently used.

