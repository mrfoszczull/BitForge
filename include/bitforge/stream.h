#ifndef BITFORGE_STREAM_H
#define BITFORGE_STREAM_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <bitforge/status.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Represents a bit-level memory stream for binary analysis.
 */
typedef struct {
	const uint8_t *data; /**< Pointer to the underlying data buffer */
	size_t size;	     /**< Total size of the buffer in bytes */
	size_t byte_offset;  /**< Current byte index */
	uint8_t bit_offset;  /**< Current bit index within the byte (0-7) */
} bf_stream_t;

/**
 * @brief Initialize a bit stream with a given data buffer.
 *
 * @param stream Pointer to the stream structure to initialize.
 * @param data Pointer to the source data buffer.
 * @param size Size of the data buffer in bytes.
 * @return bf_status BF_OK on success, or BF_EINVAL if arguments are invalid.
 */
bf_status bf_stream_init(bf_stream_t *stream, const uint8_t *data, size_t size);

/**
 * @brief Check if the stream has at least a specified number of bits remaining.
 *
 * @param stream Pointer to the bit stream.
 * @param bit_count Number of bits to check.
 * @return true if enough bits remain, false otherwise.
 */
bool bf_stream_has_remaining(const bf_stream_t *stream, size_t bit_count);

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_STREAM_H */
