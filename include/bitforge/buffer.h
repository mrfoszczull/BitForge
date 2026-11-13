#ifndef BITFORGE_BUFFER_H
#define BITFORGE_BUFFER_H

#include <bitforge/status.h>
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

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
	const uint8_t
	    *data;   /**< Pointer to the first byte of the viewed region. */
	size_t size; /**< Number of bytes in the viewed region. */
} bf_view_t;

/**
 * @brief Mutable buffer backed by caller-provided memory.
 */
typedef struct {
	uint8_t *data;	 /**< Pointer to the caller-provided memory. */
	size_t capacity; /**< Total available memory in bytes. */
	size_t size;	 /**< Number of bytes currently used. */
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

/**
 * @brief Clears the logical contents of a buffer.
 *
 * Sets the buffer's size to zero without modifying the
 * underlying memory or its capacity.
 *
 * @param buffer Buffer to clear.
 * @return BF_OK on success, BF_EINVAL if the buffer is invalid.
 */
bf_status bf_buffer_clear(bf_buffer_t *buffer);

/**
 * @brief Fills a range of a buffer with a specified byte value.
 *
 * Writes the given value into the specified range without
 * changing the buffer's logical size.
 *
 * @param buffer Destination buffer.
 * @param offset Starting offset in the buffer.
 * @param length Number of bytes to fill.
 * @param value Byte value used to fill the range.
 * @return BF_OK on success, BF_EINVAL on invalid arguments
 *         or an out-of-bounds range.
 */
bf_status bf_buffer_fill(
    bf_buffer_t *buffer, size_t offset, size_t length, uint8_t value);

/**
 * @brief Compares the contents of two memory views.
 *
 * Compares the sizes and byte contents of both views.
 *
 * @param a First memory view.
 * @param b Second memory view.
 * @param equal Output parameter set to true if both views
 *              contain identical data, false otherwise.
 * @return BF_OK on success, BF_EINVAL if any argument is invalid.
 */
bf_status bf_view_compare(const bf_view_t *a, const bf_view_t *b, bool *equal);

/**
 * @brief Copies the contents of one buffer into another.
 *
 * Copies all logically used bytes from the source buffer
 * into the destination buffer.
 *
 * @param destination Destination buffer.
 * @param source Source buffer.
 * @return BF_OK on success, BF_EINVAL if an argument is invalid
 *         or the destination capacity is insufficient.
 */
bf_status bf_buffer_copy(bf_buffer_t *destination, const bf_buffer_t *source);

/**
 * @brief Reads bytes from a buffer into a destination memory region.
 *
 * @param buffer Source buffer.
 * @param offset Starting offset in the buffer (relative to buffer->size).
 * @param destination Destination memory region.
 * @param length Number of bytes to read.
 * @return BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds
 * range.
 */
bf_status bf_buffer_read(const bf_buffer_t *buffer, size_t offset,
    uint8_t *destination, size_t length);

/**
 * @brief Inserts bytes into a buffer at the specified offset, shifting existing
 * data.
 *
 * Increases the logical size of the buffer.
 *
 * @param buffer Target buffer.
 * @param offset Offset at which to insert bytes (can be equal to buffer->size).
 * @param source Source memory to insert.
 * @param length Number of bytes to insert.
 * @return BF_OK on success, BF_EINVAL on invalid arguments, out-of-bounds
 * offset, or insufficient remaining capacity.
 */
bf_status bf_buffer_insert(
    bf_buffer_t *buffer, size_t offset, const uint8_t *source, size_t length);

/**
 * @brief Removes bytes from a buffer at the specified offset, shifting
 * subsequent data.
 *
 * Decreases the logical size of the buffer.
 *
 * @param buffer Target buffer.
 * @param offset Offset from which to remove bytes.
 * @param length Number of bytes to remove.
 * @return BF_OK on success, BF_EINVAL on invalid arguments or out-of-bounds
 * range.
 */
bf_status bf_buffer_remove(bf_buffer_t *buffer, size_t offset, size_t length);

/**
 * @brief Changes the logical size of the buffer.
 *
 * If new_size is greater than current size, the newly exposed bytes are zeroed.
 *
 * @param buffer Target buffer.
 * @param new_size New logical size in bytes.
 * @return BF_OK on success, BF_EINVAL if new_size exceeds buffer capacity or
 * arguments are invalid.
 */
bf_status bf_buffer_resize(bf_buffer_t *buffer, size_t new_size);

/**
 * @brief Checks if the buffer has sufficient capacity for additional bytes.
 *
 * @param buffer Target buffer.
 * @param additional_capacity Number of additional bytes required.
 * @return BF_OK if capacity is sufficient, BF_EINVAL on invalid args or
 * insufficient capacity.
 */
bf_status bf_buffer_reserve(
    const bf_buffer_t *buffer, size_t additional_capacity);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_BUFFER_H */
