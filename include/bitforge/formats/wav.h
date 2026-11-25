#ifndef BITFORGE_FORMATS_WAV_H
#define BITFORGE_FORMATS_WAV_H

#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup formats_wav WAV (RIFF/WAVE) Format Parser
 * @brief Zero-allocation C17 parser for RIFF/WAVE containers, `fmt `
 *        subchunks, RIFF chunk alignment, and zero-copy audio sample slices.
 *
 * Parses the 12-byte `"RIFF"` + size + `"WAVE"` header, reads RIFF subchunks
 * respecting word alignment, decodes `fmt ` parameters, and exposes zero-copy
 * views of `"data"` subchunk audio payloads over `bf_stream_t`.
 * @{
 */

/** @name WAV / RIFF Layout Constants
 * @{
 */
/** @brief Size of `"RIFF"` + 32-bit size + `"WAVE"` header in bytes. */
#define BF_WAV_RIFF_HEADER_LEN 12U
/** @brief Size of RIFF subchunk header (4-byte ID + 4-byte size) in bytes. */
#define BF_WAV_CHUNK_HEADER_LEN 8U
/** @brief Minimum byte length of a standard `fmt ` subchunk payload. */
#define BF_WAV_FMT_MIN_LEN 16U
/** @} */

/**
 * @brief Common WAVE audio format codes (`wFormatTag`).
 */
typedef enum {
	BF_WAV_FORMAT_PCM = 1U,	       /**< Uncompressed linear PCM. */
	BF_WAV_FORMAT_IEEE_FLOAT = 3U, /**< IEEE 754 floating-point samples. */
	BF_WAV_FORMAT_EXTENSIBLE = 65534U /**< `WAVE_FORMAT_EXTENSIBLE`. */
} bf_wav_audio_format_t;

/**
 * @brief Parsed RIFF/WAVE container header.
 */
typedef struct {
	uint32_t riff_size; /**< Remaining RIFF payload size after `riff_size`
			       field. */
} bf_wav_riff_header_t;

/**
 * @brief Parsed WAVE `"fmt "` subchunk fields.
 */
typedef struct {
	uint16_t audio_format; /**< Sample encoding format
				  (`bf_wav_audio_format_t`). */
	uint16_t num_channels; /**< Number of audio channels (`> 0`). */
	uint32_t sample_rate;  /**< Sample rate in Hz (`> 0`). */
	uint32_t byte_rate;    /**< Average bytes per second. */
	uint16_t block_align;  /**< Bytes per sample frame across all channels
				  (`> 0`). */
	uint16_t bits_per_sample; /**< Bits per single sample (`> 0`). */
} bf_wav_fmt_t;

/**
 * @brief Zero-copy view of a single RIFF subchunk inside the stream buffer.
 *
 * The `data` pointer references bytes directly inside `stream->data` without
 * allocating memory and remains valid while the underlying buffer stays alive.
 */
typedef struct {
	uint32_t size; /**< Declared payload length in bytes. */
	char id[5];    /**< 4-char ASCII chunk ID + NUL terminator. */
	const uint8_t
	    *data; /**< Pointer to payload bytes (`NULL` if `size == 0`). */
} bf_wav_chunk_t;

/**
 * @brief Zero-copy view of `"data"` subchunk audio samples.
 *
 * @warning The `data` pointer remains valid only while the backing stream
 *          buffer stays alive and unmodified.
 */
typedef struct {
	const uint8_t
	    *data;   /**< Pointer to sample bytes inside stream buffer. */
	size_t size; /**< Payload byte length. */
} bf_wav_data_t;

/**
 * @brief Reads and validates the 12-byte `"RIFF"` + size + `"WAVE"` header.
 *
 * @param[in,out] stream     Byte-aligned stream positioned at RIFF header.
 * @param[out]    out_header Populated RIFF header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_wav_read_riff_header(
    bf_stream_t *stream, bf_wav_riff_header_t *out_header);

/**
 * @brief Reads the next RIFF subchunk and advances past RIFF word alignment.
 *
 * Returns `BF_EOF` when at exact end of stream with 0 remaining bytes.
 *
 * @param[in,out] stream    Byte-aligned stream positioned at a RIFF subchunk.
 * @param[out]    out_chunk Populated subchunk view on success.
 * @return `BF_OK` on success, `BF_EOF`, `BF_EINVAL`, `BF_EOVERFLOW`, or
 *         `BF_EFORMAT`.
 */
bf_status bf_wav_read_chunk(bf_stream_t *stream, bf_wav_chunk_t *out_chunk);

/**
 * @brief Parses and validates a WAVE `"fmt "` subchunk from a chunk view.
 *
 * @param[in]  chunk   Subchunk view with ID `"fmt "`.
 * @param[out] out_fmt Populated WAVE format descriptor on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_wav_parse_fmt(const bf_wav_chunk_t *chunk, bf_wav_fmt_t *out_fmt);

/**
 * @brief Extracts a zero-copy sample view from a WAVE `"data"` subchunk.
 *
 * @param[in]  chunk    Subchunk view with ID `"data"`.
 * @param[out] out_data Populated zero-copy sample slice on success.
 * @return `BF_OK` on success, `BF_EINVAL` or `BF_EFORMAT`.
 */
bf_status bf_wav_get_data(const bf_wav_chunk_t *chunk, bf_wav_data_t *out_data);

/**
 * @brief Checks whether `chunk->id` equals the 4-character `four_cc` tag.
 *
 * @param[in] chunk   Subchunk view to inspect.
 * @param[in] four_cc 4-character chunk ID (e.g., `"fmt "`, `"data"`).
 * @return `true` if `chunk` and `four_cc` are valid and match, `false`
 * otherwise.
 */
bool bf_wav_chunk_is(const bf_wav_chunk_t *chunk, const char four_cc[4]);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_WAV_H */
