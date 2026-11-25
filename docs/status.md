

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

Defined in include/bitforge/status.h:20

Status codes returned by BitForge functions.

These status codes are shared across all BitForge modules and indicate whether an operation completed successfully or encountered an error.

| Value | Description |
|-------|-------------|
| `BF_OK` | Operation completed successfully. |
| `BF_EINVAL` | An argument is invalid or an object is in an invalid state. |
| `BF_ENOMEM` | Memory allocation failed. |
| `BF_EIO` | An input/output operation failed. |
| `BF_EOF` | End of input was reached before the requested operation could be completed. |
| `BF_ENOTFOUND` | The requested element, value, or pattern was not found. |
| `BF_EOVERFLOW` | An arithmetic operation or requested size exceeds the representable range. |
| `BF_EUNSUPPORTED` | The requested operation or feature is not supported. |
| `BF_EFORMAT` | Input data does not conform to the expected format or structure. |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| `const char *` | [`bf_status_string`](#bf_status_string)  | Returns a human-readable string for a status code. |

---

### bf_status_string

```cpp
const char * bf_status_string(bf_status status)
```

Defined in include/bitforge/status.h:44

Returns a human-readable string for a status code.

#### Returns
A constant string describing the status.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `status` | [`bf_status`](#bf_status) | Status code to describe. |

