#include <bitforge/pattern.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static uint64_t full_mask_for_bits(size_t bit_count)
{
	if (bit_count == 0U || bit_count > BF_MAX_PATTERN_BITS) {
		return 0ULL;
	}
	if (bit_count == 64U) {
		return 0xFFFFFFFFFFFFFFFFULL;
	}
	return (1ULL << bit_count) - 1ULL;
}

static bool pattern_is_valid(const bf_pattern_t *pattern)
{
	if (pattern == NULL) {
		return false;
	}
	if (pattern->bit_count == 0U ||
	    pattern->bit_count > BF_MAX_PATTERN_BITS) {
		return false;
	}
	return true;
}

static bool stream_state_is_valid(const bf_stream_t *stream)
{
	if (stream == NULL) {
		return false;
	}
	if (stream->data == NULL && stream->size > 0U) {
		return false;
	}
	if (stream->byte_offset > stream->size) {
		return false;
	}
	if (stream->bit_offset > 7U) {
		return false;
	}
	if (stream->byte_offset == stream->size && stream->bit_offset != 0U) {
		return false;
	}
	return true;
}

static bf_status read_window_at_position(
    const bf_stream_t *stream, size_t bit_count, uint64_t *out_value)
{
	bf_stream_t cursor = *stream;
	uint64_t value = 0ULL;

	if (!bf_stream_has_remaining(&cursor, bit_count)) {
		return BF_EOF;
	}

	for (size_t i = 0; i < bit_count; i++) {
		uint32_t bit = 0;
		bf_status status = bf_stream_read_bits(&cursor, 1U, &bit);
		if (status != BF_OK) {
			return status;
		}
		value = (value << 1) | (uint64_t)(bit & 1U);
	}

	*out_value = value;
	return BF_OK;
}

static int hex_nibble_value(char c)
{
	unsigned char uc = (unsigned char)c;
	if (uc >= '0' && uc <= '9') {
		return (int)(uc - '0');
	}
	if (uc >= 'a' && uc <= 'f') {
		return 10 + (int)(uc - 'a');
	}
	if (uc >= 'A' && uc <= 'F') {
		return 10 + (int)(uc - 'A');
	}
	return -1;
}

bf_status bf_pattern_init(
    bf_pattern_t *pattern, const uint8_t *data, size_t bit_count)
{
	if (pattern == NULL || data == NULL || bit_count == 0U ||
	    bit_count > BF_MAX_PATTERN_BITS) {
		return BF_EINVAL;
	}

	uint64_t val = 0ULL;
	for (size_t i = 0; i < bit_count; i++) {
		size_t byte_idx = i / 8U;
		size_t bit_idx = i % 8U;
		uint8_t bit =
		    (uint8_t)((data[byte_idx] >> (7U - bit_idx)) & 1U);

		val = (val << 1) | (uint64_t)bit;
	}

	uint64_t window_mask = full_mask_for_bits(bit_count);
	pattern->data = val & window_mask;
	pattern->mask = window_mask;
	pattern->bit_count = bit_count;

	return BF_OK;
}

bf_status bf_pattern_init_masked(bf_pattern_t *pattern, const uint8_t *data,
    const uint8_t *mask, size_t bit_count)
{
	if (pattern == NULL || data == NULL || mask == NULL ||
	    bit_count == 0U || bit_count > BF_MAX_PATTERN_BITS) {
		return BF_EINVAL;
	}

	uint64_t val = 0ULL;
	uint64_t mask_val = 0ULL;

	for (size_t i = 0; i < bit_count; i++) {
		size_t byte_idx = i / 8U;
		size_t bit_idx = i % 8U;

		uint8_t bit =
		    (uint8_t)((data[byte_idx] >> (7U - bit_idx)) & 1U);
		uint8_t m_bit =
		    (uint8_t)((mask[byte_idx] >> (7U - bit_idx)) & 1U);

		val = (val << 1) | (uint64_t)bit;
		mask_val = (mask_val << 1) | (uint64_t)m_bit;
	}

	uint64_t window_mask = full_mask_for_bits(bit_count);
	pattern->mask = mask_val & window_mask;
	pattern->data = val & pattern->mask;
	pattern->bit_count = bit_count;

	return BF_OK;
}

bf_status bf_pattern_parse_hex(bf_pattern_t *pattern, const char *text)
{
	if (pattern == NULL || text == NULL) {
		return BF_EINVAL;
	}

	uint64_t data_acc = 0ULL;
	uint64_t mask_acc = 0ULL;
	size_t byte_count = 0U;
	const char *p = text;

	while (*p != '\0') {
		while (*p != '\0' && isspace((unsigned char)*p)) {
			p++;
		}
		if (*p == '\0') {
			break;
		}

		if (byte_count >= (BF_MAX_PATTERN_BITS / 8U)) {
			return BF_EINVAL;
		}

		if (p[0] == '?' && p[1] == '?') {
			data_acc = (data_acc << 8);
			mask_acc = (mask_acc << 8);
			byte_count++;
			p += 2;
		} else {
			int hi = hex_nibble_value(p[0]);
			int lo = hex_nibble_value(p[1]);
			if (hi < 0 || lo < 0) {
				return BF_EINVAL;
			}
			uint8_t byte_val = (uint8_t)((hi << 4) | lo);
			data_acc = (data_acc << 8) | (uint64_t)byte_val;
			mask_acc = (mask_acc << 8) | 0xFFULL;
			byte_count++;
			p += 2;
		}

		if (*p != '\0' && !isspace((unsigned char)*p)) {
			return BF_EINVAL;
		}
	}

	if (byte_count == 0U) {
		return BF_EINVAL;
	}

	size_t bits = byte_count * 8U;
	uint64_t window_mask = full_mask_for_bits(bits);
	pattern->mask = mask_acc & window_mask;
	pattern->data = data_acc & pattern->mask;
	pattern->bit_count = bits;

	return BF_OK;
}

bf_status bf_pattern_match(
    const bf_pattern_t *pattern, const bf_stream_t *stream, bool *out_match)
{
	if (!pattern_is_valid(pattern) || !stream_state_is_valid(stream) ||
	    out_match == NULL) {
		return BF_EINVAL;
	}

	*out_match = false;

	uint64_t window = 0ULL;
	bf_status status =
	    read_window_at_position(stream, pattern->bit_count, &window);
	if (status == BF_EOF) {
		return BF_OK;
	}
	if (status != BF_OK) {
		return status;
	}

	uint64_t window_mask = full_mask_for_bits(pattern->bit_count);
	uint64_t effective_mask = pattern->mask & window_mask;
	*out_match =
	    ((window & effective_mask) == (pattern->data & effective_mask));

	return BF_OK;
}

bf_status bf_stream_find_pattern(
    bf_stream_t *stream, const bf_pattern_t *pattern)
{
	if (!stream_state_is_valid(stream) || !pattern_is_valid(pattern)) {
		return BF_EINVAL;
	}

	uint64_t window_mask = full_mask_for_bits(pattern->bit_count);
	uint64_t effective_mask = pattern->mask & window_mask;
	uint64_t expected = pattern->data & effective_mask;

	while (bf_stream_has_remaining(stream, pattern->bit_count)) {
		uint64_t window = 0ULL;
		bf_status status = read_window_at_position(
		    stream, pattern->bit_count, &window);
		if (status == BF_EOF) {
			break;
		}
		if (status != BF_OK) {
			return status;
		}

		if ((window & effective_mask) == expected) {
			return BF_OK;
		}

		status = bf_stream_skip_bits(stream, 1U);
		if (status != BF_OK) {
			return (status == BF_EOF) ? BF_ENOTFOUND : status;
		}
	}

	return BF_ENOTFOUND;
}

bf_status bf_stream_find_pattern_from(const bf_stream_t *stream,
    const bf_pattern_t *pattern, size_t start_byte, uint8_t start_bit,
    size_t *out_byte, uint8_t *out_bit)
{
	if (!stream_state_is_valid(stream) || !pattern_is_valid(pattern) ||
	    out_byte == NULL || out_bit == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t search = *stream;
	bf_status status = bf_stream_seek(&search, start_byte, start_bit);
	if (status != BF_OK) {
		return status;
	}

	status = bf_stream_find_pattern(&search, pattern);
	if (status != BF_OK) {
		return status;
	}

	return bf_stream_tell(&search, out_byte, out_bit);
}

bf_status bf_stream_find_pattern_all(bf_stream_t *stream,
    const bf_pattern_t *pattern, size_t *positions, size_t capacity,
    size_t *out_count)
{
	if (!stream_state_is_valid(stream) || !pattern_is_valid(pattern) ||
	    out_count == NULL || (positions == NULL && capacity > 0U)) {
		return BF_EINVAL;
	}

	*out_count = 0U;

	bf_stream_t search = *stream;
	size_t found = 0U;

	while (true) {
		bf_status status = bf_stream_find_pattern(&search, pattern);
		if (status == BF_ENOTFOUND) {
			break;
		}
		if (status != BF_OK) {
			return status;
		}

		if (found < capacity) {
			if (search.byte_offset > (SIZE_MAX / 8U)) {
				return BF_EOVERFLOW;
			}
			size_t match_bit_pos = (search.byte_offset * 8U) +
					       (size_t)search.bit_offset;
			positions[found] = match_bit_pos;
		}
		found++;

		status = bf_stream_skip_bits(&search, 1U);
		if (status == BF_EOF) {
			break;
		}
		if (status != BF_OK) {
			return status;
		}
	}

	*out_count = found;
	return BF_OK;
}
