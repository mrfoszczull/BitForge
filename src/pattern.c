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
