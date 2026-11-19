#ifndef BITFORGE_PATTERN_H
#define BITFORGE_PATTERN_H

#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup pattern Patterns
 * @brief Bit-level pattern matching and searching with wildcard support
 * @{
 */

/** @brief Maximum number of bits in a pattern. */
#define BF_MAX_PATTERN_BITS 64

/**
 * @brief A bit pattern with a mask.
 *
 * Bits are stored in MSB-first read order across the least significant
 * bit_count bits of data and mask (bits >= bit_count are zeroed).
 * A bit with mask bit == 1 must match data; mask bit == 0 is a wildcard.
 */
typedef struct {
	uint64_t data; /**< Expected bit values (LSB-aligned window). */
	uint64_t mask; /**< Comparison mask (1 = compare, 0 = wildcard). */
	size_t
	    bit_count; /**< Number of valid bits (1 to BF_MAX_PATTERN_BITS). */
} bf_pattern_t;

/**
 * @brief Initializes a standard bit pattern (all bits compared) from a byte
 * buffer.
 *
 * @param pattern Pointer to the pattern structure to initialize.
 * @param data Buffer containing the pattern data (MSB-first).
 * @param bit_count Number of bits to read from data (1 to BF_MAX_PATTERN_BITS).
 * @return BF_OK on success, or BF_EINVAL on invalid arguments.
 */
bf_status bf_pattern_init(
    bf_pattern_t *pattern, const uint8_t *data, size_t bit_count);

/**
 * @brief Initializes a bit pattern with a custom bitmask (supporting
 * wildcards).
 *
 * @param pattern Pointer to the pattern structure to initialize.
 * @param data Buffer containing the pattern data (MSB-first).
 * @param mask Buffer containing the bitmask (MSB-first; 1 = compare, 0 =
 * wildcard).
 * @param bit_count Number of bits to read from buffers (1 to
 * BF_MAX_PATTERN_BITS).
 * @return BF_OK on success, or BF_EINVAL on invalid arguments.
 */
bf_status bf_pattern_init_masked(bf_pattern_t *pattern, const uint8_t *data,
    const uint8_t *mask, size_t bit_count);

/**
 * @brief Parses a hexadecimal pattern string with optional "??" byte wildcards.
 *
 * Example: "48 8B ?? ?? 89 45 ??" (up to 8 bytes / 64 bits).
 *
 * @param pattern Pointer to the pattern structure to initialize.
 * @param text Null-terminated hex string.
 * @return BF_OK on success, or BF_EINVAL on invalid arguments or syntax.
 */
bf_status bf_pattern_parse_hex(bf_pattern_t *pattern, const char *text);

/**
 * @brief Checks whether the pattern matches at the current stream position.
 *
 * Does not modify the stream position.
 *
 * @param pattern Pattern to match.
 * @param stream Bit stream to inspect.
 * @param out_match Output flag set to true on match, false otherwise.
 * @return BF_OK on success (including when out_match is set to false),
 *         or BF_EINVAL on invalid arguments.
 */
bf_status bf_pattern_match(
    const bf_pattern_t *pattern, const bf_stream_t *stream, bool *out_match);

/**
 * @brief Searches the stream forward from its current position for a pattern.
 *
 * If found, stream is positioned at the start of the first match.
 * If not found, stream position is left at the end of the search.
 *
 * @param stream Bit stream to search.
 * @param pattern Pattern to find.
 * @return BF_OK if found, BF_ENOTFOUND if not found,
 *         or BF_EINVAL on invalid arguments.
 */
bf_status bf_stream_find_pattern(
    bf_stream_t *stream, const bf_pattern_t *pattern);

/**
 * @brief Searches for a pattern starting at a given byte and bit offset.
 *
 * Does not modify the caller's stream position.
 *
 * @param stream Bit stream to search.
 * @param pattern Pattern to find.
 * @param start_byte Starting byte offset in stream.
 * @param start_bit Starting bit offset within start_byte (0-7).
 * @param out_byte Output byte offset of the first match.
 * @param out_bit Output bit offset within out_byte of the first match.
 * @return BF_OK if found, BF_ENOTFOUND if not found,
 *         or BF_EINVAL on invalid arguments or start position.
 */
bf_status bf_stream_find_pattern_from(const bf_stream_t *stream,
    const bf_pattern_t *pattern, size_t start_byte, uint8_t start_bit,
    size_t *out_byte, uint8_t *out_bit);

/**
 * @brief Finds all occurrences of a pattern from the current stream position.
 *
 * Does not modify the caller's stream position. Positions are recorded as
 * bit offsets from the beginning of the underlying data buffer. Up to
 * capacity positions are written to positions, while *out_count receives the
 * total number of matches found.
 *
 * @param stream Bit stream to search.
 * @param pattern Pattern to find.
 * @param positions Output array of bit offsets (may be NULL if capacity == 0).
 * @param capacity Maximum number of entries that can be stored in positions.
 * @param out_count Receives total number of matches found.
 * @return BF_OK on success, or BF_EINVAL on invalid arguments.
 */
bf_status bf_stream_find_pattern_all(const bf_stream_t *stream,
    const bf_pattern_t *pattern, size_t *positions, size_t capacity,
    size_t *out_count);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_PATTERN_H */
