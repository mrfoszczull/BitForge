

# Status codes

Status codes and error handling.

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_status`](#bf_status)  | Status codes returned by BitForge functions. |

---

### bf_status

```cpp
enum bf_status
```

Defined in include/bitforge/status.h:17

Status codes returned by BitForge functions.

| Value | Description |
|-------|-------------|
| `BF_OK` | Operation completed successfully. |
| `BF_EINVAL` | An argument is invalid. |
| `BF_ENOMEM` | Memory allocation failed. |
| `BF_EIO` | An input/output operation failed. |
| `BF_EOF` | End of input was reached. |
| `BF_ENOTFOUND` | Pattern is not found. |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`bf_status_string`](#bf_status_string)  | Returns a human-readable string for a status code. |

---

### bf_status_string

```cpp
const char * bf_status_string(bf_status status)
```

Defined in include/bitforge/status.h:32

Returns a human-readable string for a status code.

#### Returns
A constant string describing the status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | [`bf_status`](#bf_status) | Status code to describe. |

