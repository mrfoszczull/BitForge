#include <bitforge/pattern.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

bf_status bf_pattern_init(
    bf_pattern_t *pattern, const uint8_t *data, size_t bit_count)
{
	if (pattern == NULL || data == NULL || bit_count == 0 ||
	    bit_count > BF_MAX_PATTERN_BITS) {
		return BF_EINVAL;
	}

	uint64_t val = 0;
	for (size_t i = 0; i < bit_count; i++) {
		size_t byte_idx = i / 8;
		size_t bit_idx = i % 8;
		uint8_t current_byte = data[byte_idx];
		uint8_t bit = (current_byte >> (7 - bit_idx)) & 1;

		val = (val << 1) | bit;
	}

	pattern->data = val;
	pattern->mask = (bit_count == 64) ? 0xFFFFFFFFFFFFFFFFULL
					  : ((1ULL << bit_count) - 1);
	pattern->bit_count = bit_count;

	return BF_OK;
}

bf_status bf_pattern_init_masked(bf_pattern_t *pattern, const uint8_t *data,
    const uint8_t *mask, size_t bit_count)
{
	if (pattern == NULL || data == NULL || mask == NULL || bit_count == 0 ||
	    bit_count > BF_MAX_PATTERN_BITS) {
		return BF_EINVAL;
	}

	uint64_t val = 0;
	uint64_t mask_val = 0;

	for (size_t i = 0; i < bit_count; i++) {
		size_t byte_idx = i / 8;
		size_t bit_idx = i % 8;

		uint8_t d_byte = data[byte_idx];
		uint8_t m_byte = mask[byte_idx];

		uint8_t bit = (d_byte >> (7 - bit_idx)) & 1;
		uint8_t m_bit = (m_byte >> (7 - bit_idx)) & 1;

		val = (val << 1) | bit;
		mask_val = (mask_val << 1) | m_bit;
	}

	pattern->data = val & mask_val;
	pattern->mask = mask_val;
	pattern->bit_count = bit_count;

	return BF_OK;
}

bf_status bf_stream_find_pattern(
    bf_stream_t *stream, const bf_pattern_t *pattern)
{
	if (stream == NULL || pattern == NULL || pattern->bit_count == 0 ||
	    pattern->bit_count > BF_MAX_PATTERN_BITS) {
		return BF_EINVAL;
	}

	while (bf_stream_has_remaining(stream, pattern->bit_count)) {
		bf_stream_t test_stream = *stream;
		uint64_t value = 0;

		for (size_t i = 0; i < pattern->bit_count; i++) {
			uint32_t bit = 0;

			if (bf_stream_read_bits(&test_stream, 1, &bit) !=
			    BF_OK) {
				return BF_ENOTFOUND;
			}

			value = (value << 1) | (uint64_t)(bit & 1U);
		}

		if ((value & pattern->mask) ==
		    (pattern->data & pattern->mask)) {
			return BF_OK;
		}

		if (bf_stream_skip_bits(stream, 1) != BF_OK) {
			return BF_ENOTFOUND;
		}
	}

	return BF_ENOTFOUND;
}
