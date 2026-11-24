

# BMP Format Parser

Zero-allocation C17 parser for BMP headers, DIB `BITMAPINFOHEADER`, row stride calculation, and zero-copy pixel slices.

Parses the 2-byte `"BM"` signature, 14-byte BMP file header, `BITMAPINFOHEADER` (`>= 40` bytes), packed 4-byte aligned row strides, and zero-copy pixel slices over a [`bf_stream_t`](stream.md#bf_stream_t).

## Classes

| Name | Description |
|------|-------------|
| [`bf_bmp_file_header_t`](#bf_bmp_file_header_t) | Parsed BMP file header (`BITMAPFILEHEADER`, 14 bytes). |
| [`bf_bmp_info_header_t`](#bf_bmp_info_header_t) | Parsed DIB `BITMAPINFOHEADER` fields (40-byte core). |
| [`bf_bmp_pixels_t`](#bf_bmp_pixels_t) | Zero-copy view of pixel data inside the underlying stream buffer. |

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_bmp_compression_t`](#bf_bmp_compression_t)  | Supported BMP compression modes (`biCompression`). |

---

### bf_bmp_compression_t

```cpp
enum bf_bmp_compression_t
```

Defined in include/bitforge/formats/bmp.h:38

Supported BMP compression modes (`biCompression`).

| Value | Description |
|-------|-------------|
| `BF_BMP_COMP_RGB` | Uncompressed RGB (`BI_RGB`). |
| `BF_BMP_COMP_RLE8` | 8-bit run-length encoding (`BI_RLE8`). |
| `BF_BMP_COMP_RLE4` | 4-bit run-length encoding (`BI_RLE4`). |
| `BF_BMP_COMP_BITFIELDS` | Bitfield masks (`BI_BITFIELDS`). |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_bmp_read_signature`](#bf_bmp_read_signature)  | Verifies the 2-byte BMP signature (`"BM"`) without advancing on mismatch. |
| [`bf_status`](status.md#bf_status) | [`bf_bmp_read_file_header`](#bf_bmp_read_file_header)  | Reads and validates the 14-byte BMP file header. |
| [`bf_status`](status.md#bf_status) | [`bf_bmp_read_info_header`](#bf_bmp_read_info_header)  | Reads and validates a `BITMAPINFOHEADER` (`>= 40` bytes). |
| [`bf_status`](status.md#bf_status) | [`bf_bmp_row_stride`](#bf_bmp_row_stride)  | Computes row stride in bytes for uncompressed packed rows. |
| [`bf_status`](status.md#bf_status) | [`bf_bmp_get_pixels`](#bf_bmp_get_pixels)  | Seeks to `pixel_offset` and returns a zero-copy pixel slice view. |

---

### bf_bmp_read_signature

```cpp
bf_status bf_bmp_read_signature(bf_stream_t * stream)
```

Defined in include/bitforge/formats/bmp.h:100

Verifies the 2-byte BMP signature (`"BM"`) without advancing on mismatch.

#### Returns
[`BF_OK`](status.md#bf_status) on match, [`BF_EINVAL`](status.md#bf_status) for invalid stream, [`BF_EOF`](status.md#bf_status) on truncated input, or [`BF_EFORMAT`](status.md#bf_status) on mismatch.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at the BMP signature. |

---

### bf_bmp_read_file_header

```cpp
bf_status bf_bmp_read_file_header(bf_stream_t * stream, bf_bmp_file_header_t * out_header)
```

Defined in include/bitforge/formats/bmp.h:109

Reads and validates the 14-byte BMP file header.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at file header. |
| `out_header` | [`bf_bmp_file_header_t`](#bf_bmp_file_header_t) * | Populated BMP file header on success. |

---

### bf_bmp_read_info_header

```cpp
bf_status bf_bmp_read_info_header(bf_stream_t * stream, bf_bmp_info_header_t * out_info)
```

Defined in include/bitforge/formats/bmp.h:122

Reads and validates a `BITMAPINFOHEADER` (`>= 40` bytes).

Advances [`Streams`](stream.md#streams) to `byte_offset + header_size` on success.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at DIB header. |
| `out_info` | [`bf_bmp_info_header_t`](#bf_bmp_info_header_t) * | Populated DIB info header on success. |

---

### bf_bmp_row_stride

```cpp
bf_status bf_bmp_row_stride(uint32_t width, uint16_t bit_count, size_t * out_stride)
```

Defined in include/bitforge/formats/bmp.h:136

Computes row stride in bytes for uncompressed packed rows.

Row size is padded to a 4-byte boundary: `((width * bit_count + 31) / 32) * 4`.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), or [`BF_EOVERFLOW`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `width` | `uint32_t` | Image width in pixels. |
| `bit_count` | `uint16_t` | Bits per pixel. |
| `out_stride` | `size_t *` | Computed row stride in bytes on success. |

---

### bf_bmp_get_pixels

```cpp
bf_status bf_bmp_get_pixels(bf_stream_t * stream, const bf_bmp_file_header_t * file_hdr, const bf_bmp_info_header_t * info_hdr, bf_bmp_pixels_t * out_pixels)
```

Defined in include/bitforge/formats/bmp.h:149

Seeks to `pixel_offset` and returns a zero-copy pixel slice view.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream backing the BMP image. |
| `file_hdr` | const [`bf_bmp_file_header_t`](#bf_bmp_file_header_t) * | Parsed BMP file header. |
| `info_hdr` | const [`bf_bmp_info_header_t`](#bf_bmp_info_header_t) * | Parsed DIB info header. |
| `out_pixels` | [`bf_bmp_pixels_t`](#bf_bmp_pixels_t) * | Populated zero-copy pixel view on success. |


## Class Definitions



### bf_bmp_file_header_t

```cpp
#include <bmp.h>
```

```cpp
struct bf_bmp_file_header_t
```

Defined in include/bitforge/formats/bmp.h:48

Parsed BMP file header (`BITMAPFILEHEADER`, 14 bytes).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`file_size`](#file_size)  | Declared total file size in bytes. |
| `uint16_t` | [`reserved1`](#reserved1)  | Reserved field 1. |
| `uint16_t` | [`reserved2`](#reserved2)  | Reserved field 2. |
| `uint32_t` | [`pixel_offset`](#pixel_offset)  | Byte offset from start of file to pixel array. |

---

##### file_size

```cpp
uint32_t file_size
```

Defined in include/bitforge/formats/bmp.h:49

Declared total file size in bytes.

---

##### reserved1

```cpp
uint16_t reserved1
```

Defined in include/bitforge/formats/bmp.h:50

Reserved field 1.

---

##### reserved2

```cpp
uint16_t reserved2
```

Defined in include/bitforge/formats/bmp.h:51

Reserved field 2.

---

##### pixel_offset

```cpp
uint32_t pixel_offset
```

Defined in include/bitforge/formats/bmp.h:53

Byte offset from start of file to pixel array.



### bf_bmp_info_header_t

```cpp
#include <bmp.h>
```

```cpp
struct bf_bmp_info_header_t
```

Defined in include/bitforge/formats/bmp.h:59

Parsed DIB `BITMAPINFOHEADER` fields (40-byte core).

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`header_size`](#header_size)  | DIB header size in bytes (`>= 40`). |
| `int32_t` | [`width`](#width)  | Image width in pixels (`> 0`). |
| `int32_t` | [`height`](#height)  | Image height (`> 0` bottom-up, `< 0` top-down). |
| `uint16_t` | [`planes`](#planes)  | Color planes count (must be `1`). |
| `uint16_t` | [`bit_count`](#bit_count-1)  | Bits per pixel (`1`, `4`, `8`, `16`, `24`, `32`). |
| `uint32_t` | [`compression`](#compression)  | Compression method ([`bf_bmp_compression_t`](#bf_bmp_compression_t)). |
| `uint32_t` | [`image_size`](#image_size)  | Pixel data size in bytes (may be `0` for `BI_RGB`). |
| `int32_t` | [`x_pixels_per_meter`](#x_pixels_per_meter)  | Horizontal resolution in pixels per meter. |
| `int32_t` | [`y_pixels_per_meter`](#y_pixels_per_meter)  | Vertical resolution in pixels per meter. |
| `uint32_t` | [`colors_used`](#colors_used)  | Number of color table entries used. |
| `uint32_t` | [`colors_important`](#colors_important)  | Number of important colors. |

---

##### header_size

```cpp
uint32_t header_size
```

Defined in include/bitforge/formats/bmp.h:60

DIB header size in bytes (`>= 40`).

---

##### width

```cpp
int32_t width
```

Defined in include/bitforge/formats/bmp.h:61

Image width in pixels (`> 0`).

---

##### height

```cpp
int32_t height
```

Defined in include/bitforge/formats/bmp.h:62

Image height (`> 0` bottom-up, `< 0` top-down).

---

##### planes

```cpp
uint16_t planes
```

Defined in include/bitforge/formats/bmp.h:63

Color planes count (must be `1`).

---

##### bit_count

```cpp
uint16_t bit_count
```

Defined in include/bitforge/formats/bmp.h:65

Bits per pixel (`1`, `4`, `8`, `16`, `24`, `32`).

---

##### compression

```cpp
uint32_t compression
```

Defined in include/bitforge/formats/bmp.h:67

Compression method ([`bf_bmp_compression_t`](#bf_bmp_compression_t)).

---

##### image_size

```cpp
uint32_t image_size
```

Defined in include/bitforge/formats/bmp.h:68

Pixel data size in bytes (may be `0` for `BI_RGB`).

---

##### x_pixels_per_meter

```cpp
int32_t x_pixels_per_meter
```

Defined in include/bitforge/formats/bmp.h:70

Horizontal resolution in pixels per meter.

---

##### y_pixels_per_meter

```cpp
int32_t y_pixels_per_meter
```

Defined in include/bitforge/formats/bmp.h:73

Vertical resolution in pixels per meter.

---

##### colors_used

```cpp
uint32_t colors_used
```

Defined in include/bitforge/formats/bmp.h:74

Number of color table entries used.

---

##### colors_important

```cpp
uint32_t colors_important
```

Defined in include/bitforge/formats/bmp.h:75

Number of important colors.



### bf_bmp_pixels_t

```cpp
#include <bmp.h>
```

```cpp
struct bf_bmp_pixels_t
```

Defined in include/bitforge/formats/bmp.h:84

Zero-copy view of pixel data inside the underlying stream buffer.

:::warning
The [`data`](#data-5) pointer remains valid only while the backing stream buffer stays alive and unmodified. 

:::

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`data`](#data-5)  | Pointer to pixel bytes inside stream buffer. |
| `size_t` | [`size`](#size-5)  | Available pixel byte count from `pixel_offset`. |
| `bool` | [`top_down`](#top_down)  | `true` when `height < 0`. |
| `uint32_t` | [`abs_height`](#abs_height)  | Positive pixel row count. |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/formats/bmp.h:86

Pointer to pixel bytes inside stream buffer.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/formats/bmp.h:87

Available pixel byte count from `pixel_offset`.

---

##### top_down

```cpp
bool top_down
```

Defined in include/bitforge/formats/bmp.h:88

`true` when `height < 0`.

---

##### abs_height

```cpp
uint32_t abs_height
```

Defined in include/bitforge/formats/bmp.h:89

Positive pixel row count.

