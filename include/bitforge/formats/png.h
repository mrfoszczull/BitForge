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
 * @brief Zero-allocation C17 parser for PNG signatures, `IHDR` headers,
 *        chunk streams, and CRC-32 verification.
 *
 * Parses the 8-byte PNG signature, validates `IHDR`, iterates over PNG chunks
 * (`length`, `type`, zero-copy `data` slice, big-endian `crc`), and computes
 * or verifies ISO 3309 / IEEE 802.3 CRC-32 checksums over `bf_stream_t`.
 * @{
 */

/** @name PNG Layout Constants
 * @{
 */
/** @brief Size of standard PNG file signature in bytes. */
#define BF_PNG_SIG_LEN 8U
/** @brief Length of ASCII chunk type identifier in bytes. */
#define BF_PNG_CHUNK_TYPE_LEN 4U
/** @brief Length of trailing big-endian chunk CRC-32 in bytes. */
#define BF_PNG_CHUNK_CRC_LEN 4U
/** @brief Fixed payload byte length of a valid `IHDR` chunk. */
#define BF_PNG_IHDR_DATA_LEN 13U
/** @} */

/**
 * @brief Parsed PNG `IHDR` header fields.
 */
typedef struct {
	uint32_t width;	     /**< Image width in pixels (`> 0`). */
	uint32_t height;     /**< Image height in pixels (`> 0`). */
	uint8_t bit_depth;   /**< Bits per sample or palette index. */
	uint8_t color_type;  /**< PNG color type (`0`, `2`, `3`, `4`, `6`). */
	uint8_t compression; /**< Compression method (`0` = deflate/inflate). */
	uint8_t filter;	     /**< Filter method (`0` = adaptive filtering). */
	uint8_t interlace;   /**< Interlace method (`0` = none, `1` = Adam7). */
} bf_png_ihdr_t;

/**
 * @brief Zero-copy view of a single PNG chunk inside the stream buffer.
 *
 * The `data` pointer references bytes directly inside `stream->data` without
 * allocating memory and remains valid while the underlying buffer stays alive.
 */
typedef struct {
	uint32_t length; /**< Length of chunk data payload in bytes. */
	char type[5];	 /**< 4-char ASCII chunk type + NUL terminator. */
	const uint8_t
	    *data; /**< Pointer to payload bytes (`NULL` if `length == 0`). */
	uint32_t crc; /**< Big-endian CRC-32 stored at end of chunk. */
} bf_png_chunk_t;

/**
 * @brief Verifies and consumes the 8-byte PNG signature at current stream
 *        offset.
 *
 * Leaves `stream` position unchanged on mismatch (`BF_EFORMAT`).
 *
 * @param[in,out] stream Byte-aligned stream positioned at PNG signature.
 * @return `BF_OK` on match, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_png_read_signature(bf_stream_t *stream);

/**
 * @brief Reads and validates the PNG `IHDR` chunk (length, type, payload, CRC).
 *
 * Expects `stream` positioned right after the 8-byte PNG signature (or at
 * `IHDR`), and advances past `IHDR` CRC on success.
 *
 * @param[in,out] stream   Byte-aligned stream positioned at `IHDR` chunk.
 * @param[out]    out_ihdr Populated `IHDR` header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 *         `BF_EFORMAT`.
 */
bf_status bf_png_read_ihdr(bf_stream_t *stream, bf_png_ihdr_t *out_ihdr);

/**
 * @brief Reads the next PNG chunk from the stream.
 *
 * Advances `stream` past `length` (4 B), `type` (4 B), `data` (`length` B),
 * and `crc` (4 B); returns `BF_EOF` at exact end of stream with 0 remaining
 * bytes.
 *
 * @param[in,out] stream    Byte-aligned stream positioned at a PNG chunk.
 * @param[out]    out_chunk Populated chunk view on success.
 * @return `BF_OK` on success, `BF_EOF`, `BF_EINVAL`, `BF_EOVERFLOW`, or
 *         `BF_EFORMAT`.
 */
bf_status bf_png_read_chunk(bf_stream_t *stream, bf_png_chunk_t *out_chunk);

/**
 * @brief Computes standard PNG CRC-32 (ISO 3309 / IEEE 802.3) over a byte
 *        slice.
 *
 * @param[in] data   Input byte buffer (may be `NULL` only when `length == 0`).
 * @param[in] length Number of bytes to hash.
 * @return Computed 32-bit CRC checksum.
 */
uint32_t bf_png_crc32(const uint8_t *data, size_t length);

/**
 * @brief Verifies chunk CRC-32 against chunk `type` (4 bytes) and payload
 *        `data`.
 *
 * @param[in] chunk Chunk view to verify.
 * @return `BF_OK` if matching, `BF_EFORMAT` on mismatch, or `BF_EINVAL` on
 *         invalid arguments.
 */
bf_status bf_png_verify_chunk_crc(const bf_png_chunk_t *chunk);

/**
 * @brief Checks whether `chunk->type` matches the 4-character `four_cc` tag.
 *
 * @param[in] chunk   Chunk view to inspect.
 * @param[in] four_cc 4-character chunk type code (e.g., `"IHDR"`, `"IDAT"`,
 * `"IEND"`).
 * @return `true` if `chunk` and `four_cc` are valid and match, `false`
 * otherwise.
 */
bool bf_png_chunk_is(const bf_png_chunk_t *chunk, const char four_cc[4]);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_PNG_H */
