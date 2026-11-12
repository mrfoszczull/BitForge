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

/**
 * @brief Reads bytes from a memory view.
 *
 * @param view Source memory view.
 * @param offset Starting offset in the view.
 * @param destination Destination memory.
 * @param length Number of bytes to read.
 * @return BF_OK on success, BF_EINVAL on invalid arguments.
 */
bf_status bf_view_read(
    const bf_view_t *view, size_t offset, uint8_t *destination, size_t length);

/**
 * @brief Creates a read-only subview without copying data.
 *
 * @param view Source memory view.
 * @param offset Starting offset in the source view.
 * @param length Number of bytes in the subview.
 * @param result Destination view.
 * @return BF_OK on success, BF_EINVAL on invalid arguments.
 */
bf_status bf_view_slice(
    const bf_view_t *view, size_t offset, size_t length, bf_view_t *result);

/**
 * @brief Writes bytes into a buffer at the specified offset.
 *
 * Does not change the buffer's logical size.
 *
 * @param buffer Destination buffer.
 * @param offset Starting offset in the buffer.
 * @param source Source memory.
 * @param length Number of bytes to write.
 * @return BF_OK on success, BF_EINVAL on invalid arguments.
 */
bf_status bf_buffer_write(
    bf_buffer_t *buffer, size_t offset, const uint8_t *source, size_t length);

/**
 * @brief Creates a read-only view of the buffer's current contents.
 *
 * @param buffer Source buffer.
 * @param view Destination view.
 * @return BF_OK on success, BF_EINVAL on invalid arguments.
 */
bf_status bf_buffer_get_view(const bf_buffer_t *buffer, bf_view_t *view);

/**
 * @brief Appends bytes to the end of a buffer.
 *
 * Copies data to the buffer starting at its current logical size
 * and increases the size after a successful operation.
 *
 * @param buffer Destination buffer.
 * @param source Source memory.
 * @param length Number of bytes to append.
 * @return BF_OK on success, BF_EINVAL on invalid arguments or
 *         insufficient buffer capacity.
 */
bf_status bf_buffer_append(
    bf_buffer_t *buffer, const uint8_t *source, size_t length);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_BUFFER_H */
