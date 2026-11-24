

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
| [`bf_status`](status.md#bf_status) | [`bf_buffer_write`](#bf_buffer_write)  | Writes bytes into a buffer at the specified offset within capacity. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_get_view`](#bf_buffer_get_view)  | Creates a read-only view of the buffer's current logical contents. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_append`](#bf_buffer_append)  | Appends bytes to the end of a buffer and increases its logical size. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_clear`](#bf_buffer_clear)  | Clears the logical contents of a buffer. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_fill`](#bf_buffer_fill)  | Fills a range within buffer capacity with a specified byte value. |
| [`bf_status`](status.md#bf_status) | [`bf_view_compare`](#bf_view_compare)  | Compares the logical contents of two memory views. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_copy`](#bf_buffer_copy)  | Copies the logical contents of one buffer into another. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_read`](#bf_buffer_read)  | Reads bytes from the buffer's logical contents ([0, buffer->size)). |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_insert`](#bf_buffer_insert)  | Inserts bytes into a buffer at the specified logical offset. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_remove`](#bf_buffer_remove)  | Removes bytes from a buffer at the specified logical offset. |
| [`bf_status`](status.md#bf_status) | [`bf_buffer_resize`](#bf_buffer_resize)  | Changes the logical size of the buffer. |
| `bool` | [`bf_buffer_has_capacity`](#bf_buffer_has_capacity)  | Checks whether the buffer has enough remaining capacity. |

---

### bf_view_init

```cpp
bf_status bf_view_init(bf_view_t * view, const uint8_t * data, size_t size)
```

Defined in include/bitforge/buffer.h:44

Initializes a read-only memory view.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `view` | [`bf_view_t`](#bf_view_t) * | Destination view. |
| `data` | `const uint8_t *` | Source memory (may be NULL only when size == 0). |
| `size` | `size_t` | Number of bytes available. |

---

### bf_buffer_init

```cpp
bf_status bf_buffer_init(bf_buffer_t * buffer, uint8_t * data, size_t capacity)
```

Defined in include/bitforge/buffer.h:56

Initializes a writable buffer using caller-provided memory.

The buffer starts empty (size == 0). No memory is allocated.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `data` | `uint8_t *` | Writable memory region (may be NULL only when capacity == 0). |
| `capacity` | `size_t` | Total available capacity in bytes. |

---

### bf_view_read

```cpp
bf_status bf_view_read(const bf_view_t * view, size_t offset, uint8_t * destination, size_t length)
```

Defined in include/bitforge/buffer.h:68

Reads bytes from a memory view.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `view` | const [`bf_view_t`](#bf_view_t) * | Source memory view. |
| `offset` | `size_t` | Starting byte offset within the view. |
| `destination` | `uint8_t *` | Destination memory. |
| `length` | `size_t` | Number of bytes to read. |

---

### bf_view_slice

```cpp
bf_status bf_view_slice(const bf_view_t * view, size_t offset, size_t length, bf_view_t * result)
```

Defined in include/bitforge/buffer.h:81

Creates a read-only subview without copying data.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `view` | const [`bf_view_t`](#bf_view_t) * | Source memory view. |
| `offset` | `size_t` | Starting byte offset within the source view. |
| `length` | `size_t` | Number of bytes in the subview. |
| `result` | [`bf_view_t`](#bf_view_t) * | Destination view. |

---

### bf_buffer_write

```cpp
bf_status bf_buffer_write(bf_buffer_t * buffer, size_t offset, const uint8_t * source, size_t length)
```

Defined in include/bitforge/buffer.h:97

Writes bytes into a buffer at the specified offset within capacity.

Supports overlapping source and destination memory regions. Does not change the buffer's logical size.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `offset` | `size_t` | Starting byte offset in the buffer. |
| `source` | `const uint8_t *` | Source memory. |
| `length` | `size_t` | Number of bytes to write. |

---

### bf_buffer_get_view

```cpp
bf_status bf_buffer_get_view(const bf_buffer_t * buffer, bf_view_t * view)
```

Defined in include/bitforge/buffer.h:107

Creates a read-only view of the buffer's current logical contents.

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

Defined in include/bitforge/buffer.h:120

Appends bytes to the end of a buffer and increases its logical size.

Supports overlapping source and destination memory regions.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or insufficient capacity.

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

Defined in include/bitforge/buffer.h:131

Clears the logical contents of a buffer.

Sets the buffer's logical size to zero without modifying underlying memory.

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

Fills a range within buffer capacity with a specified byte value.

Does not change the buffer's logical size.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Destination buffer. |
| `offset` | `size_t` | Starting byte offset in the buffer. |
| `length` | `size_t` | Number of bytes to fill. |
| `value` | `uint8_t` | Byte value used to fill the range. |

---

### bf_view_compare

```cpp
bf_status bf_view_compare(const bf_view_t * a, const bf_view_t * b, bool * equal)
```

Defined in include/bitforge/buffer.h:157

Compares the logical contents of two memory views.

#### Returns
BF_OK on success, BF_EINVAL if any argument is invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `a` | const [`bf_view_t`](#bf_view_t) * | First memory view. |
| `b` | const [`bf_view_t`](#bf_view_t) * | Second memory view. |
| `equal` | `bool *` | Output parameter set to true if both views have equal size and identical bytes. |

---

### bf_buffer_copy

```cpp
bf_status bf_buffer_copy(bf_buffer_t * destination, const bf_buffer_t * source)
```

Defined in include/bitforge/buffer.h:169

Copies the logical contents of one buffer into another.

Supports overlapping source and destination memory regions.

#### Returns
BF_OK on success, BF_EINVAL if an argument is invalid or destination capacity is insufficient.

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

Defined in include/bitforge/buffer.h:181

Reads bytes from the buffer's logical contents ([0, buffer->size)).

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | const [`bf_buffer_t`](#bf_buffer_t) * | Source buffer. |
| `offset` | `size_t` | Starting byte offset from the beginning of logical data. |
| `destination` | `uint8_t *` | Destination memory region. |
| `length` | `size_t` | Number of bytes to read. |

---

### bf_buffer_insert

```cpp
bf_status bf_buffer_insert(bf_buffer_t * buffer, size_t offset, const uint8_t * source, size_t length)
```

Defined in include/bitforge/buffer.h:196

Inserts bytes into a buffer at the specified logical offset.

Shifts existing tail data and increases logical size by length.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, out-of-bounds offset, or insufficient capacity.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Target buffer. |
| `offset` | `size_t` | Byte offset in [0, buffer->size] at which to insert bytes. |
| `source` | `const uint8_t *` | Source memory to insert. |
| `length` | `size_t` | Number of bytes to insert. |

---

### bf_buffer_remove

```cpp
bf_status bf_buffer_remove(bf_buffer_t * buffer, size_t offset, size_t length)
```

Defined in include/bitforge/buffer.h:210

Removes bytes from a buffer at the specified logical offset.

Shifts subsequent data and decreases logical size by length.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds range.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Target buffer. |
| `offset` | `size_t` | Starting byte offset in [0, buffer->size]. |
| `length` | `size_t` | Number of bytes to remove. |

---

### bf_buffer_resize

```cpp
bf_status bf_buffer_resize(bf_buffer_t * buffer, size_t new_size)
```

Defined in include/bitforge/buffer.h:223

Changes the logical size of the buffer.

If new_size is greater than current size, newly exposed bytes are zeroed.

#### Returns
BF_OK on success, BF_EINVAL if new_size exceeds capacity or arguments are invalid.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | [`bf_buffer_t`](#bf_buffer_t) * | Target buffer. |
| `new_size` | `size_t` | New logical size in bytes (must be less than or equal to capacity). |

---

### bf_buffer_has_capacity

```cpp
bool bf_buffer_has_capacity(const bf_buffer_t * buffer, size_t additional_capacity)
```

Defined in include/bitforge/buffer.h:233

Checks whether the buffer has enough remaining capacity.

#### Returns
true if buffer is valid and capacity - size is greater than or equal to additional_capacity, false otherwise.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `buffer` | const [`bf_buffer_t`](#bf_buffer_t) * | Target buffer. |
| `additional_capacity` | `size_t` | Number of additional bytes required. |


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
| `const uint8_t *` | [`data`](#data)  |  |
| `size_t` | [`size`](#size)  |  |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/buffer.h:23

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/buffer.h:24



### bf_buffer_t

```cpp
#include <buffer.h>
```

```cpp
struct bf_buffer_t
```

Defined in include/bitforge/buffer.h:30

Mutable buffer backed by caller-provided memory.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint8_t *` | [`data`](#data-1)  |  |
| `size_t` | [`capacity`](#capacity)  |  |
| `size_t` | [`size`](#size-1)  |  |

---

##### data

```cpp
uint8_t * data
```

Defined in include/bitforge/buffer.h:31

---

##### capacity

```cpp
size_t capacity
```

Defined in include/bitforge/buffer.h:32

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/buffer.h:33

