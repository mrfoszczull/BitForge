

# Buffers

Memory views and caller-managed buffers.

## Classes

| Name | Description |
|------|-------------|
| [`bf_view_t`](#bf_view_t) | Read-only view of a memory region. |
| [`bf_buffer_t`](#bf_buffer_t) | Writable buffer backed by caller-provided memory. |

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

---

### bf_view_init

```cpp
bf_status bf_view_init(bf_view_t * view, const uint8_t * data, size_t size)
```

Defined in include/bitforge/buffer.h:43

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

Defined in include/bitforge/buffer.h:55

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

Defined in include/bitforge/buffer.h:66

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

Defined in include/bitforge/buffer.h:78

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

Defined in include/bitforge/buffer.h:92

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

Defined in include/bitforge/buffer.h:102

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

Defined in include/bitforge/buffer.h:116

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


## Class Definitions



### bf_view_t

```cpp
#include <buffer.h>
```

```cpp
struct bf_view_t
```

Defined in include/bitforge/buffer.h:21

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

Defined in include/bitforge/buffer.h:22

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/buffer.h:23



### bf_buffer_t

```cpp
#include <buffer.h>
```

```cpp
struct bf_buffer_t
```

Defined in include/bitforge/buffer.h:29

Writable buffer backed by caller-provided memory.

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

Defined in include/bitforge/buffer.h:30

---

##### capacity

```cpp
size_t capacity
```

Defined in include/bitforge/buffer.h:31

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/buffer.h:32

