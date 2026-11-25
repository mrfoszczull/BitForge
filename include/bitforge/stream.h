#ifndef BITFORGE_STREAM_H
#define BITFORGE_STREAM_H

#include <bitforge/status.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup stream Streams
 * @brief Sequential MSB-first bit and byte stream over a memory buffer
 * @{
 */

/**
 * @brief Represents a bit-level memory stream for binary analysis.
 */
typedef struct {
	const uint8_t *data; /**< Pointer to the underlying data buffer. */
	size_t size;	     /**< Total size of the buffer in bytes. */
	size_t byte_offset;  /**< Current byte index in [0, size]. */
	uint8_t
	    bit_offset; /**< Current bit index within byte (0-7, MSB-first). */
} bf_stream_t;

/**
 * @brief Initializes a bit stream with a given data buffer.
 *
 * @param stream Pointer to the stream structure to initialize.
 * @param data Pointer to the source data buffer (may be NULL only when size ==
 * 0).
 * @param size Size of the data buffer in bytes.
 * @return BF_OK on success, or BF_EINVAL if arguments are invalid.
 */
bf_status bf_stream_init(bf_stream_t *stream, const uint8_t *data, size_t size);

/**
 * @brief Checks if the stream has at least a specified number of bits
 * remaining.
 *
 * @param stream Pointer to the bit stream.
 * @param bit_count Number of bits to check.
 * @return true if the stream is valid and at least bit_count bits remain,
 *         false otherwise.
 */
bool bf_stream_has_remaining(const bf_stream_t *stream, size_t bit_count);

/**
 * @brief Reads 1 to 32 bits from the stream in MSB-first order.
 *
 * Extracted bits are placed in the least significant bit_count bits of
 * *out_value. Stream position is unchanged on error.
 *
 * @param stream Pointer to the bit stream.
 * @param bit_count Number of bits to read (1 to 32).
 * @param out_value Pointer to store the read value.
 * @return BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if fewer
 *         than bit_count bits remain.
 */
bf_status bf_stream_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value);

/**
 * @brief Reads a single full byte (8 bits) in MSB-first order.
 *
 * @param stream Pointer to the bit stream.
 * @param out_value Pointer to store the read byte.
 * @return BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if fewer
 *         than 8 bits remain.
 */
bf_status bf_stream_read_byte(bf_stream_t *stream, uint8_t *out_value);

/**
 * @brief Reads multiple bytes from a byte-aligned stream.
 *
 * Requires stream->bit_offset == 0.
 *
 * @param stream Pointer to the bit stream.
 * @param destination Destination buffer (may be NULL only when length == 0).
 * @param length Number of bytes to read.
 * @return BF_OK on success, BF_EINVAL on invalid arguments or unaligned stream,
 *         or BF_EOF if fewer than length bytes remain.
 */
bf_status bf_stream_read_bytes(
    bf_stream_t *stream, uint8_t *destination, size_t length);

/**
 * @brief Reads a single bit as a boolean value (MSB-first).
 *
 * @param stream Pointer to the bit stream.
 * @param out_value Pointer to store the boolean result.
 * @return BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if no
 *         bits remain.
 */
bf_status bf_stream_read_bool(bf_stream_t *stream, bool *out_value);

/**
 * @brief Advances the stream position by bit_count bits.
 *
 * Position is unchanged if fewer than bit_count bits remain.
 *
 * @param stream Pointer to the bit stream.
 * @param bit_count Number of bits to skip.
 * @return BF_OK on success, BF_EINVAL on invalid arguments, or BF_EOF if fewer
 *         than bit_count bits remain.
 */
bf_status bf_stream_skip_bits(bf_stream_t *stream, size_t bit_count);

/**
 * @brief Aligns the stream to the next byte boundary.
 *
 * If bit_offset != 0, advances to bit_offset == 0 at the next byte.
 *
 * @param stream Pointer to the bit stream.
 * @return BF_OK on success, or BF_EINVAL on invalid arguments or stream state.
 */
bf_status bf_stream_align(bf_stream_t *stream);

/**
 * @brief Sets the stream position to a specific byte and bit offset.
 *
 * @param stream Pointer to the bit stream.
 * @param byte_offset Target byte offset in [0, stream->size].
 * @param bit_offset Target bit offset in [0, 7] (must be 0 when
 *                   byte_offset == stream->size).
 * @return BF_OK on success, BF_EINVAL if position is invalid or out of bounds.
 */
bf_status bf_stream_seek(
    bf_stream_t *stream, size_t byte_offset, uint8_t bit_offset);

/**
 * @brief Gets the current byte and bit offset of the stream.
 *
 * @param stream Pointer to the bit stream.
 * @param out_byte_offset Optional pointer to receive current byte offset.
 * @param out_bit_offset Optional pointer to receive current bit offset (0-7).
 * @return BF_OK on success, or BF_EINVAL on invalid arguments.
 */
bf_status bf_stream_tell(const bf_stream_t *stream, size_t *out_byte_offset,
    uint8_t *out_bit_offset);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_STREAM_H */
