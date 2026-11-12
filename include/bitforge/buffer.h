#ifndef BITFORGE_BUFFER_H
#define BITFORGE_BUFFER_H

#include <bitforge/status.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup buffer Buffers
 * @brief Memory views and caller-managed buffers.
 * @{
 */

/**
 * @brief Read-only view of a memory region.
 */
typedef struct {
	const uint8_t *data;
	size_t size;
} bf_view_t;

/**
 * @brief Writable buffer backed by caller-provided memory.
 */
typedef struct {
	uint8_t *data;
	size_t capacity;
	size_t size;
} bf_buffer_t;

/**
 * @brief Initializes a read-only memory view.
 *
 * @param view Destination view.
 * @param data Source memory.
 * @param size Number of bytes available.
 * @return BF_OK on success, BF_EINVAL on invalid arguments.
 */
bf_status bf_view_init(bf_view_t *view, const uint8_t *data, size_t size);

/**
 * @brief Initializes a writable buffer using caller-provided memory.
 *
 * The buffer starts empty. No memory is allocated.
 *
 * @param buffer Destination buffer.
 * @param data Writable memory region.
 * @param capacity Total available capacity.
 * @return BF_OK on success, BF_EINVAL on invalid arguments.
 */
bf_status bf_buffer_init(bf_buffer_t *buffer, uint8_t *data, size_t capacity);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_BUFFER_H */
