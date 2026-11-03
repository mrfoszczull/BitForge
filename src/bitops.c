#include <bitforge/bitops.h>
#include <bitforge/status.h>
#include <stddef.h>
#include <stdint.h>

bf_status bf_get(const uint8_t *value, size_t bit_index, bool *result)
{
	if (value == NULL || bit_index >= 8 || result == NULL) {
		return BF_EINVAL;
	}

	*result = (*value >> bit_index) & 1;

	return BF_OK;
}

bf_status bf_set(uint8_t *value, size_t bit_index)
{
	if (value == NULL || bit_index >= 8) {
		return BF_EINVAL;
	}

	*value |= (1u << bit_index);

	return BF_OK;
}

bf_status bf_clear(uint8_t *value, size_t bit_index)
{
	if (value == NULL || bit_index >= 8) {
		return BF_EINVAL;
	}
	*value &= ~(1u << bit_index);

	return BF_OK;
}

bf_status bf_toggle(uint8_t *value, size_t bit_index)
{
	if (value == NULL || bit_index >= 8) {
		return BF_EINVAL;
	}

	*value ^= (1u << bit_index);

	return BF_OK;
}

bf_status bf_count_set_bits(const uint8_t *value, uint8_t *result)
{
	if (value == NULL || result == NULL) {
		return BF_EINVAL;
	}

	*result = 0;
	for (int i = 0; i < 8; i++) {
		if ((*value >> i) & 1u) {
			(*result)++;
		}
	}

	return BF_OK;
}

bf_status bf_is_power_of_two(uint8_t value, bool *result)
{
	if (result == NULL) {
		return BF_EINVAL;
	}

	if (value == 0) {
		*result = false;
	} else {
		*result = ((value & (value - 1)) == 0);
	}

	return BF_OK;
}

bf_status bf_reverse_bits(uint8_t value, uint8_t *result)
{
	if (result == NULL) {
		return BF_EINVAL;
	}

	int reversed = 0;

	for (int i = 0; i < 8; i++) {
		uint8_t bit = value & 1u;
		reversed = (reversed << 1) | bit;
		value >>= 1;
	}

	*result = reversed;

	return BF_OK;
}

bf_status bf_rotate_left(uint8_t value, unsigned int position, uint8_t *result)
{
	if (result == NULL) {
		return BF_EINVAL;
	}

	position %= 8;
	if (position == 0) {
		*result = value;
		return BF_OK;
	}

	uint8_t rotation_result =
	    (value << position) | (value >> (8 - position));
	*result = rotation_result;

	return BF_OK;
}

bf_status bf_rotate_right(uint8_t value, unsigned int position, uint8_t *result)
{
	if (result == NULL) {
		return BF_EINVAL;
	}

	position %= 8;
	if (position == 0) {
		*result = value;
		return BF_OK;
	}

	uint8_t rotation_result =
	    (value >> position) | (value << (8 - position));
	*result = rotation_result;

	return BF_OK;
}

bf_status bf_get_bits(
    uint8_t value, size_t bit_index, size_t bit_count, uint8_t *result)
{
	if (result == NULL || bit_count == 0 || bit_count > 8) {
		return BF_EINVAL;
	}

	if (bit_index > 7 || bit_count > 8 - bit_index) {
		return BF_EINVAL;
	}

	*result = (value >> bit_index) & ((1U << bit_count) - 1);

	return BF_OK;
}

bf_status bf_set_bits(uint8_t *value, size_t bit_index, size_t bit_count)
{
	if (value == NULL || bit_count == 0 || bit_count > 8) {
		return BF_EINVAL;
	}

	if (bit_index > 7 || bit_count > 8 - bit_index) {
		return BF_EINVAL;
	}

	size_t mask = ((size_t)1 << bit_count) - 1;
	mask <<= bit_index;

	*value |= mask;

	return BF_OK;
}

bf_status bf_clear_bits(uint8_t *value, size_t bit_index, size_t bit_count)
{
	if (value == NULL || bit_count == 0 || bit_count > 8) {
		return BF_EINVAL;
	}

	if (bit_index > 7 || bit_count > 8 - bit_index) {
		return BF_EINVAL;
	}

	size_t mask = ((size_t)1 << bit_count) - 1;
	mask <<= bit_index;

	*value &= ~mask;

	return BF_OK;
}

bf_status bf_toggle_bits(uint8_t *value, size_t bit_index, size_t bit_count)
{
	if (value == NULL || bit_count == 0 || bit_count > 8) {
		return BF_EINVAL;
	}

	if (bit_index > 7 || bit_count > 8 - bit_index) {
		return BF_EINVAL;
	}

	size_t mask = ((size_t)1 << bit_count) - 1;
	mask <<= bit_index;

	*value ^= mask;

	return BF_OK;
}

bf_status bf_swap_bits(uint8_t *value, size_t first_bit, size_t second_bit)
{
	if (value == NULL || first_bit > 7 || second_bit > 7) {
		return BF_EINVAL;
	}

	uint8_t first_value = (*value >> first_bit) & 1U;
	uint8_t second_value = (*value >> second_bit) & 1U;

	if (first_value != second_value) {
		*value ^= (uint8_t)((1U << first_bit) | (1U << second_bit));
	}

	return BF_OK;
}
