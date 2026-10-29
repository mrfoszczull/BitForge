#ifndef BITOPS_H
#define BITOPS_H

#include <bitforge/status.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @brief Reads a bit from an 8-bit value.
 *
 * @param value Pointer to the value to read.
 * @param bit_index Bit index in the range [0, 7], where 0 is the least
 * significant bit.
 * @param result Pointer to receive the bit value.
 * @return BF_OK on success, BF_EINVAL if a pointer is NULL or bit_index
 * is outside [0, 7].
 */
bf_status bf_get(const uint8_t *value, size_t bit_index, bool *result);

/**
 * @brief Sets a bit in an 8-bit value.
 *
 * @param value Pointer to the value to modify.
 * @param bit_index Bit index in the range [0, 7].
 * @return BF_OK on success, BF_EINVAL if value is NULL or bit_index
 * is outside [0, 7].
 */
bf_status bf_set(uint8_t *value, size_t bit_index);

/**
 * @brief Clears a bit in an 8-bit value.
 *
 * @param value Pointer to the value to modify.
 * @param bit_index Bit index in the range [0, 7].
 * @return BF_OK on success, BF_EINVAL if value is NULL or bit_index
 * is outside [0, 7].
 */
bf_status bf_clear(uint8_t *value, size_t bit_index);

/**
 * @brief Toggles a bit in an 8-bit value.
 *
 * @param value Pointer to the value to modify.
 * @param bit_index Bit index in the range [0, 7].
 * @return BF_OK on success, BF_EINVAL if value is NULL or bit_index
 * is outside [0, 7].
 */
bf_status bf_toggle(uint8_t *value, size_t bit_index);

/**
 * @brief Counts the set bits in an 8-bit value.
 *
 * @param value Pointer to the value to examine.
 * @param result Pointer to receive the number of set bits (0–8).
 * @return BF_OK on success, BF_EINVAL if value or result is NULL.
 */
bf_status bf_count_set_bits(const uint8_t *value, uint8_t *result);

/**
 * @brief Checks whether a value is a power of two.
 *
 * Zero is not considered a power of two.
 *
 * @param value Value to check.
 * @param result Pointer to receive the result.
 * @return BF_OK on success, BF_EINVAL if result is NULL.
 */
bf_status bf_is_power_of_two(uint8_t value, bool *result);

/**
 * @brief Reverses the order of the bits in an 8-bit value.
 *
 * @param value Value whose bits are to be reversed.
 * @param result Pointer to receive the reversed value.
 * @return BF_OK on success, BF_EINVAL if result is NULL.
 */
bf_status bf_reverse_bits(uint8_t value, uint8_t *result);

/**
 * @brief Rotates an 8-bit value to the left.
 *
 * The rotation count is reduced modulo 8.
 *
 * @param value Value to rotate.
 * @param position Number of bit positions to rotate.
 * @param result Pointer to receive the rotated value.
 * @return BF_OK on success, BF_EINVAL if result is NULL.
 */
bf_status bf_rotate_left(uint8_t value, unsigned int position, uint8_t *result);

#endif
