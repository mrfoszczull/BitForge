#ifndef BITFORGE_PATTERN_H
#define BITFORGE_PATTERN_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stddef.h>
#include <stdint.h>

#define BF_MAX_PATTERN_BITS 64

typedef struct {
	uint64_t data;	  // Normalized pattern value
	uint64_t mask;	  // Bitmask (1 = check bit, 0 = wildcard)
	size_t bit_count; // Exact pattern length in bits (1 - 64)
} bf_pattern_t;

/**
 * @brief Initializes a standard bit pattern from a raw byte buffer.
 *
 * @param pattern Pointer to the pattern structure to initialize.
 * @param data Buffer containing the pattern data.
 * @param bit_count Number of bits to read from the buffer (max
 * BF_MAX_PATTERN_BITS).
 * @return BF_OK on success, or BF_EINVAL on invalid arguments.
 */
bf_status bf_pattern_init(
    bf_pattern_t *pattern, const uint8_t *data, size_t bit_count);

/**
 * @brief Initializes a bit pattern with a custom bitmask (supporting
 * wildcards).
 *
 * @param pattern Pointer to the pattern structure to initialize.
 * @param data Buffer containing the pattern data.
 * @param mask Buffer containing the bitmask (1 = check bit, 0 = wildcard).
 * @param bit_count Number of bits to read from buffers (max
 * BF_MAX_PATTERN_BITS).
 * @return BF_OK on success, or BF_EINVAL on invalid arguments.
 */
bf_status bf_pattern_init_masked(bf_pattern_t *pattern, const uint8_t *data,
    const uint8_t *mask, size_t bit_count);

/**
 * @brief Searches the bit stream for the first occurrence of a pattern.
 *
 * @param stream Pointer to the bit stream to search.
 * @param pattern Pointer to the bit pattern to find.
 * @return BF_OK if found (stream is positioned at the start of the match),
 *         BF_ENOTFOUND if the pattern is not found, or BF_EINVAL on invalid
 * arguments.
 */
bf_status bf_stream_find_pattern(
    bf_stream_t *stream, const bf_pattern_t *pattern);

#ifdef __cplusplus
}
#endif

#endif // BITFORGE_PATTERN_H
