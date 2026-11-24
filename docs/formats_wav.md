

# WAV (RIFF/WAVE) Format Parser

Zero-allocation C17 parser for RIFF/WAVE containers, `fmt ` subchunks, RIFF chunk alignment, and zero-copy audio sample slices.

Parses the 12-byte `"RIFF"` + size + `"WAVE"` header, reads RIFF subchunks respecting word alignment, decodes `fmt ` parameters, and exposes zero-copy views of `"data"` subchunk audio payloads over [`bf_stream_t`](stream.md#bf_stream_t).

## Classes

| Name | Description |
|------|-------------|
| [`bf_wav_riff_header_t`](#bf_wav_riff_header_t) | Parsed RIFF/WAVE container header. |
| [`bf_wav_fmt_t`](#bf_wav_fmt_t) | Parsed WAVE `"fmt "` subchunk fields. |
| [`bf_wav_chunk_t`](#bf_wav_chunk_t) | Zero-copy view of a single RIFF subchunk inside the stream buffer. |
| [`bf_wav_data_t`](#bf_wav_data_t) | Zero-copy view of `"data"` subchunk audio samples. |

## Enumerations

| Name | Description |
|------|-------------|
| [`bf_wav_audio_format_t`](#bf_wav_audio_format_t)  | Common WAVE audio format codes (`wFormatTag`). |

---

### bf_wav_audio_format_t

```cpp
enum bf_wav_audio_format_t
```

Defined in include/bitforge/formats/wav.h:40

Common WAVE audio format codes (`wFormatTag`).

| Value | Description |
|-------|-------------|
| `BF_WAV_FORMAT_PCM` | Uncompressed linear PCM. |
| `BF_WAV_FORMAT_IEEE_FLOAT` | IEEE 754 floating-point samples. |
| `BF_WAV_FORMAT_EXTENSIBLE` | `WAVE_FORMAT_EXTENSIBLE`. |
## Functions

| Return | Name | Description |
|--------|------|-------------|
| [`bf_status`](status.md#bf_status) | [`bf_wav_read_riff_header`](#bf_wav_read_riff_header)  | Reads and validates the 12-byte `"RIFF"` + size + `"WAVE"` header. |
| [`bf_status`](status.md#bf_status) | [`bf_wav_read_chunk`](#bf_wav_read_chunk)  | Reads the next RIFF subchunk and advances past RIFF word alignment. |
| [`bf_status`](status.md#bf_status) | [`bf_wav_parse_fmt`](#bf_wav_parse_fmt)  | Parses and validates a WAVE `"fmt "` subchunk from a chunk view. |
| [`bf_status`](status.md#bf_status) | [`bf_wav_get_data`](#bf_wav_get_data)  | Extracts a zero-copy sample view from a WAVE `"data"` subchunk. |
| `bool` | [`bf_wav_chunk_is`](#bf_wav_chunk_is)  | Checks whether `chunk->id` equals the 4-character `four_cc` tag. |

---

### bf_wav_read_riff_header

```cpp
bf_status bf_wav_read_riff_header(bf_stream_t * stream, bf_wav_riff_header_t * out_header)
```

Defined in include/bitforge/formats/wav.h:100

Reads and validates the 12-byte `"RIFF"` + size + `"WAVE"` header.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at RIFF header. |
| `out_header` | [`bf_wav_riff_header_t`](#bf_wav_riff_header_t) * | Populated RIFF header on success. |

---

### bf_wav_read_chunk

```cpp
bf_status bf_wav_read_chunk(bf_stream_t * stream, bf_wav_chunk_t * out_chunk)
```

Defined in include/bitforge/formats/wav.h:113

Reads the next RIFF subchunk and advances past RIFF word alignment.

Returns [`BF_EOF`](status.md#bf_status) when at exact end of stream with 0 remaining bytes.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EOF`](status.md#bf_status), [`BF_EINVAL`](status.md#bf_status), [`BF_EOVERFLOW`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `stream` | [`bf_stream_t`](stream.md#bf_stream_t) * | Byte-aligned stream positioned at a RIFF subchunk. |
| `out_chunk` | [`bf_wav_chunk_t`](#bf_wav_chunk_t) * | Populated subchunk view on success. |

---

### bf_wav_parse_fmt

```cpp
bf_status bf_wav_parse_fmt(const bf_wav_chunk_t * chunk, bf_wav_fmt_t * out_fmt)
```

Defined in include/bitforge/formats/wav.h:122

Parses and validates a WAVE `"fmt "` subchunk from a chunk view.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status), [`BF_EOF`](status.md#bf_status), or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `chunk` | const [`bf_wav_chunk_t`](#bf_wav_chunk_t) * | Subchunk view with ID `"fmt "`. |
| `out_fmt` | [`bf_wav_fmt_t`](#bf_wav_fmt_t) * | Populated WAVE format descriptor on success. |

---

### bf_wav_get_data

```cpp
bf_status bf_wav_get_data(const bf_wav_chunk_t * chunk, bf_wav_data_t * out_data)
```

Defined in include/bitforge/formats/wav.h:131

Extracts a zero-copy sample view from a WAVE `"data"` subchunk.

#### Returns
[`BF_OK`](status.md#bf_status) on success, [`BF_EINVAL`](status.md#bf_status) or [`BF_EFORMAT`](status.md#bf_status).

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `chunk` | const [`bf_wav_chunk_t`](#bf_wav_chunk_t) * | Subchunk view with ID `"data"`. |
| `out_data` | [`bf_wav_data_t`](#bf_wav_data_t) * | Populated zero-copy sample slice on success. |

---

### bf_wav_chunk_is

```cpp
bool bf_wav_chunk_is(const bf_wav_chunk_t * chunk, const char four_cc)
```

Defined in include/bitforge/formats/wav.h:141

Checks whether `chunk->id` equals the 4-character `four_cc` tag.

#### Returns
`true` if `chunk` and `four_cc` are valid and match, `false` otherwise.

#### Parameters

| Parameter | Type | Description |
|-----------|------|-------------|
| `chunk` | const [`bf_wav_chunk_t`](#bf_wav_chunk_t) * | Subchunk view to inspect. |
| `four_cc` | `const char` | 4-character chunk ID (e.g., `"fmt "`, `"data"`). |


## Class Definitions



### bf_wav_riff_header_t

```cpp
#include <wav.h>
```

```cpp
struct bf_wav_riff_header_t
```

Defined in include/bitforge/formats/wav.h:49

Parsed RIFF/WAVE container header.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`riff_size`](#riff_size)  | Remaining RIFF payload size after [`riff_size`](#riff_size) field. |

---

##### riff_size

```cpp
uint32_t riff_size
```

Defined in include/bitforge/formats/wav.h:50

Remaining RIFF payload size after [`riff_size`](#riff_size) field.



### bf_wav_fmt_t

```cpp
#include <wav.h>
```

```cpp
struct bf_wav_fmt_t
```

Defined in include/bitforge/formats/wav.h:57

Parsed WAVE `"fmt "` subchunk fields.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint16_t` | [`audio_format`](#audio_format)  | Sample encoding format ([`bf_wav_audio_format_t`](#bf_wav_audio_format_t)). |
| `uint16_t` | [`num_channels`](#num_channels)  | Number of audio channels (`> 0`). |
| `uint32_t` | [`sample_rate`](#sample_rate)  | Sample rate in Hz (`> 0`). |
| `uint32_t` | [`byte_rate`](#byte_rate)  | Average bytes per second. |
| `uint16_t` | [`block_align`](#block_align)  | Bytes per sample frame across all channels (`> 0`). |
| `uint16_t` | [`bits_per_sample`](#bits_per_sample)  | Bits per single sample (`> 0`). |

---

##### audio_format

```cpp
uint16_t audio_format
```

Defined in include/bitforge/formats/wav.h:58

Sample encoding format ([`bf_wav_audio_format_t`](#bf_wav_audio_format_t)).

---

##### num_channels

```cpp
uint16_t num_channels
```

Defined in include/bitforge/formats/wav.h:60

Number of audio channels (`> 0`).

---

##### sample_rate

```cpp
uint32_t sample_rate
```

Defined in include/bitforge/formats/wav.h:61

Sample rate in Hz (`> 0`).

---

##### byte_rate

```cpp
uint32_t byte_rate
```

Defined in include/bitforge/formats/wav.h:62

Average bytes per second.

---

##### block_align

```cpp
uint16_t block_align
```

Defined in include/bitforge/formats/wav.h:63

Bytes per sample frame across all channels (`> 0`).

---

##### bits_per_sample

```cpp
uint16_t bits_per_sample
```

Defined in include/bitforge/formats/wav.h:65

Bits per single sample (`> 0`).



### bf_wav_chunk_t

```cpp
#include <wav.h>
```

```cpp
struct bf_wav_chunk_t
```

Defined in include/bitforge/formats/wav.h:74

Zero-copy view of a single RIFF subchunk inside the stream buffer.

The [`data`](#data-8) pointer references bytes directly inside `stream->data` without allocating memory and remains valid while the underlying buffer stays alive.

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `uint32_t` | [`size`](#size-9)  | Declared payload length in bytes. |
| `char` | [`id`](#id)  | 4-char ASCII chunk ID + NUL terminator. |
| `const uint8_t *` | [`data`](#data-8)  | Pointer to payload bytes (`NULL` if `size == 0`). |

---

##### size

```cpp
uint32_t size
```

Defined in include/bitforge/formats/wav.h:75

Declared payload length in bytes.

---

##### id

```cpp
char id[5]
```

Defined in include/bitforge/formats/wav.h:76

4-char ASCII chunk ID + NUL terminator.

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/formats/wav.h:78

Pointer to payload bytes (`NULL` if `size == 0`).



### bf_wav_data_t

```cpp
#include <wav.h>
```

```cpp
struct bf_wav_data_t
```

Defined in include/bitforge/formats/wav.h:87

Zero-copy view of `"data"` subchunk audio samples.

:::warning
The [`data`](#data-9) pointer remains valid only while the backing stream buffer stays alive and unmodified. 

:::

#### Public Attributes

| Return | Name | Description |
|--------|------|-------------|
| `const uint8_t *` | [`data`](#data-9)  | Pointer to sample bytes inside stream buffer. |
| `size_t` | [`size`](#size-10)  | Payload byte length. |

---

##### data

```cpp
const uint8_t * data
```

Defined in include/bitforge/formats/wav.h:89

Pointer to sample bytes inside stream buffer.

---

##### size

```cpp
size_t size
```

Defined in include/bitforge/formats/wav.h:90

Payload byte length.

