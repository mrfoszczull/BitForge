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
 * @brief RIFF/WAVE container, fmt subchunk, and zero-copy data slice parser
 * @{
 */

#define BF_WAV_RIFF_HEADER_LEN 12U
#define BF_WAV_CHUNK_HEADER_LEN 8U
#define BF_WAV_FMT_MIN_LEN 16U

typedef enum {
	BF_WAV_FORMAT_PCM = 1U,
	BF_WAV_FORMAT_IEEE_FLOAT = 3U,
	BF_WAV_FORMAT_EXTENSIBLE = 65534U
} bf_wav_audio_format_t;

/**
 * @brief Parsed RIFF/WAVE container header.
 */
typedef struct {
	uint32_t riff_size; /**< Remaining RIFF payload size after riff_size
			       field. */
} bf_wav_riff_header_t;

/**
 * @brief Parsed WAVE "fmt " subchunk fields.
 */
typedef struct {
	uint16_t audio_format; /**< Sample encoding format
				  (bf_wav_audio_format_t). */
	uint16_t num_channels; /**< Number of audio channels (> 0). */
	uint32_t sample_rate;  /**< Sample rate in Hz (> 0). */
	uint32_t byte_rate;    /**< Average bytes per second. */
	uint16_t block_align; /**< Bytes per sample frame across all channels (>
				 0). */
	uint16_t bits_per_sample; /**< Bits per single sample (> 0). */
} bf_wav_fmt_t;

/**
 * @brief View of a single RIFF subchunk inside the stream buffer.
 *
 * The data pointer references bytes directly inside stream->data without
 * allocating memory. Valid while the underlying buffer remains alive.
 */
typedef struct {
	uint32_t size; /**< Declared payload length in bytes. */
	char id[5];    /**< 4-char ASCII chunk ID + NUL terminator. */
	const uint8_t
	    *data; /**< Pointer to payload bytes (may be NULL if size == 0). */
} bf_wav_chunk_t;

/**
 * @brief Zero-copy view of "data" subchunk audio samples.
 */
typedef struct {
	const uint8_t
	    *data;   /**< Pointer to sample bytes inside stream buffer. */
	size_t size; /**< Payload byte length. */
} bf_wav_data_t;

/**
 * @brief Reads and validates the 12-byte "RIFF" + size + "WAVE" header.
 */
bf_status bf_wav_read_riff_header(
    bf_stream_t *stream, bf_wav_riff_header_t *out_header);

/**
 * @brief Reads the next RIFF subchunk and advances past RIFF word alignment.
 *
 * Returns BF_EOF when at exact end of stream with 0 remaining bytes.
 */
bf_status bf_wav_read_chunk(bf_stream_t *stream, bf_wav_chunk_t *out_chunk);

/**
 * @brief Parses and validates a WAVE "fmt " subchunk from a chunk view.
 */
bf_status bf_wav_parse_fmt(const bf_wav_chunk_t *chunk, bf_wav_fmt_t *out_fmt);

/**
 * @brief Extracts a zero-copy sample view from a WAVE "data" subchunk.
 */
bf_status bf_wav_get_data(const bf_wav_chunk_t *chunk, bf_wav_data_t *out_data);

/**
 * @brief Helper returning true if chunk ID equals four_cc (e.g., "fmt ",
 * "data").
 */
bool bf_wav_chunk_is(const bf_wav_chunk_t *chunk, const char four_cc[4]);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_WAV_H */
