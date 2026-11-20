#ifndef BITFORGE_FORMATS_PNG_H
#define BITFORGE_FORMATS_PNG_H

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
 * @defgroup formats_png PNG Format Parser
 * @brief Signature, IHDR header, and chunk stream parser for PNG files
 * @{
 */

#define BF_PNG_SIG_LEN 8U
#define BF_PNG_CHUNK_TYPE_LEN 4U
#define BF_PNG_CHUNK_CRC_LEN 4U
#define BF_PNG_IHDR_DATA_LEN 13U

/**
 * @brief Parsed PNG IHDR header fields.
 */
typedef struct {
	uint32_t width;	     /**< Image width in pixels (> 0). */
	uint32_t height;     /**< Image height in pixels (> 0). */
	uint8_t bit_depth;   /**< Bits per sample or palette index. */
	uint8_t color_type;  /**< PNG color type (0, 2, 3, 4, 6). */
	uint8_t compression; /**< Compression method (0 = deflate/inflate). */
	uint8_t filter;	     /**< Filter method (0 = adaptive filtering). */
	uint8_t interlace;   /**< Interlace method (0 = none, 1 = Adam7). */
} bf_png_ihdr_t;

/**
 * @brief View of a single PNG chunk located inside the stream buffer.
 *
 * The data pointer references bytes directly inside stream->data without
 * allocating memory. Valid while the underlying buffer remains alive.
 */
typedef struct {
	uint32_t length; /**< Length of chunk data payload in bytes. */
	char type[5];	 /**< 4-char ASCII chunk type + NUL terminator. */
	const uint8_t *
	    data; /**< Pointer to payload bytes (may be NULL if length == 0). */
	uint32_t crc; /**< Big-endian CRC32 stored at end of chunk. */
} bf_png_chunk_t;

/**
 * @brief Verifies and consumes the 8-byte PNG signature at current stream
 * offset.
 *
 * Returns BF_EFORMAT if signature does not match (stream position unchanged).
 */
bf_status bf_png_read_signature(bf_stream_t *stream);

/**
 * @brief Reads and validates the PNG IHDR chunk (length, type, payload, CRC).
 *
 * Expects stream positioned right after the 8-byte PNG signature (or at IHDR).
 * Leaves stream positioned immediately after the IHDR chunk's CRC on success.
 */
bf_status bf_png_read_ihdr(bf_stream_t *stream, bf_png_ihdr_t *out_ihdr);

/**
 * @brief Reads the next PNG chunk from the stream.
 *
 * Advances stream past length (4B), type (4B), data (length B), and CRC (4B).
 * Returns BF_EOF when at exact end of stream with 0 remaining bytes.
 */
bf_status bf_png_read_chunk(bf_stream_t *stream, bf_png_chunk_t *out_chunk);

/**
 * @brief Computes standard PNG CRC-32 (ISO 3309 / IEEE 802.3) over a byte
 * slice.
 */
uint32_t bf_png_crc32(const uint8_t *data, size_t length);

/**
 * @brief Verifies chunk CRC-32 against chunk type (4 bytes) and chunk payload.
 *
 * Returns BF_OK if matching, BF_EFORMAT on mismatch, or BF_EINVAL on invalid
 * args.
 */
bf_status bf_png_verify_chunk_crc(const bf_png_chunk_t *chunk);

/**
 * @brief Helper returning true if chunk type equals four_cc (e.g., "IHDR",
 * "IDAT", "IEND").
 */
bool bf_png_chunk_is(const bf_png_chunk_t *chunk, const char four_cc[4]);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_PNG_H */
