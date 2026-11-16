

# File I/O

File input and output operations.

## Classes

| Name | Description |
|------|-------------|
| [`bf_file_t`](#bf_file_t) | Represents an opened file. |

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_file_mode`](#bf_file_mode)  | File opening modes. |

---

### bf_file_mode

```cpp
enum bf_file_mode
```

Defined in include/bitforge/io.h:33

File opening modes.

| Value | Description |
|-------|-------------|
| `BF_FILE_READ` | Open an existing file for reading. |
| `BF_FILE_WRITE` | Open for writing, truncating existing content. |
| `BF_FILE_APPEND` | Open for appending data. |
| `BF_FILE_READ_WRITE` | Open for reading and writing. |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_io_open`](#bf_io_open)  | Opens a file. |
| [`bf_status`](status.md#bf_status) | [`bf_io_close`](#bf_io_close)  | Closes an opened file. |
| [`bf_status`](status.md#bf_status) | [`bf_io_read`](#bf_io_read)  | Reads data from a file. |
| [`bf_status`](status.md#bf_status) | [`bf_io_write`](#bf_io_write)  | Writes data to a file. |
| [`bf_status`](status.md#bf_status) | [`bf_io_seek`](#bf_io_seek)  | Changes the current file position. |
| [`bf_status`](status.md#bf_status) | [`bf_io_tell`](#bf_io_tell)  | Gets the current file position. |
| [`bf_status`](status.md#bf_status) | [`bf_io_size`](#bf_io_size)  | Gets the file size. |
| `bool` | [`bf_io_is_open`](#bf_io_is_open)  | Checks whether a file is currently open. |

---

### bf_io_open

```cpp
bf_status bf_io_open(bf_file_t * file, const char * path, bf_file_mode mode)
```

Defined in include/bitforge/io.h:49

Opens a file.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | [`bf_file_t`](#bf_file_t) * | Pointer to the file structure. |
| `path` | `const char *` | Path to the file. |
| `mode` | [`bf_file_mode`](#bf_file_mode) | File opening mode. |

---

### bf_io_close

```cpp
bf_status bf_io_close(bf_file_t * file)
```

Defined in include/bitforge/io.h:58

Closes an opened file.

#### Returns
BF_OK on success, BF_EINVAL on invalid arguments, or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | [`bf_file_t`](#bf_file_t) * | Pointer to the file structure. |

---

### bf_io_read

```cpp
bf_status bf_io_read(bf_file_t * file, uint8_t * data, size_t size, size_t * out_read)
```

Defined in include/bitforge/io.h:70

Reads data from a file.

#### Returns
BF_OK on success, BF_EOF when no more data is available, or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | [`bf_file_t`](#bf_file_t) * | Pointer to the opened file. |
| `data` | `uint8_t *` | Destination buffer. |
| `size` | `size_t` | Maximum number of bytes to read. |
| `out_read` | `size_t *` | Number of bytes actually read. |

---

### bf_io_write

```cpp
bf_status bf_io_write(bf_file_t * file, const uint8_t * data, size_t size, size_t * out_written)
```

Defined in include/bitforge/io.h:82

Writes data to a file.

#### Returns
BF_OK on success or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | [`bf_file_t`](#bf_file_t) * | Pointer to the opened file. |
| `data` | `const uint8_t *` | Source buffer. |
| `size` | `size_t` | Number of bytes to write. |
| `out_written` | `size_t *` | Number of bytes actually written. |

---

### bf_io_seek

```cpp
bf_status bf_io_seek(bf_file_t * file, size_t offset)
```

Defined in include/bitforge/io.h:92

Changes the current file position.

#### Returns
BF_OK on success or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | [`bf_file_t`](#bf_file_t) * | Pointer to the opened file. |
| `offset` | `size_t` | Target offset in bytes. |

---

### bf_io_tell

```cpp
bf_status bf_io_tell(const bf_file_t * file, size_t * out_position)
```

Defined in include/bitforge/io.h:101

Gets the current file position.

#### Returns
BF_OK on success or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | const [`bf_file_t`](#bf_file_t) * | Pointer to the opened file. |
| `out_position` | `size_t *` | Pointer to receive the current position. |

---

### bf_io_size

```cpp
bf_status bf_io_size(bf_file_t * file, size_t * out_size)
```

Defined in include/bitforge/io.h:110

Gets the file size.

#### Returns
BF_OK on success or an appropriate error status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | [`bf_file_t`](#bf_file_t) * | Pointer to the opened file. |
| `out_size` | `size_t *` | Pointer to receive the file size. |

---

### bf_io_is_open

```cpp
bool bf_io_is_open(const bf_file_t * file)
```

Defined in include/bitforge/io.h:118

Checks whether a file is currently open.

#### Returns
true if the file is open, false otherwise.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `file` | const [`bf_file_t`](#bf_file_t) * | Pointer to the file structure. |


## Class Definitions



### bf_file_t

```cpp
#include <io.h>
```

```cpp
struct bf_file_t
```

Defined in include/bitforge/io.h:26

Represents an opened file.

The internal file handle is managed by the BitForge I/O module.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `FILE *` | [`handle`](#handle)  |  |

---

##### handle

```cpp
FILE * handle
```

Defined in include/bitforge/io.h:27

