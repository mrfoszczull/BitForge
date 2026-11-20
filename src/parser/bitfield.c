#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

static bool stream_state_valid(const bf_stream_t *stream)
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

bf_status bf_parser_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value)
{
	if (!stream_state_valid(stream) || out_value == NULL ||
	    bit_count == 0U || bit_count > 32U) {
		return BF_EINVAL;
	}

	return bf_stream_read_bits(stream, bit_count, out_value);
}

bf_status bf_parser_read_flag(bf_stream_t *stream, bool *out_flag)
{
	if (!stream_state_valid(stream) || out_flag == NULL) {
		return BF_EINVAL;
	}

	return bf_stream_read_bool(stream, out_flag);
}

bf_status bf_parser_check_reserved_bits(bf_stream_t *stream, size_t bit_count)
{
	if (!stream_state_valid(stream)) {
		return BF_EINVAL;
	}

	if (bit_count == 0U) {
		return BF_OK;
	}

	if (!bf_stream_has_remaining(stream, bit_count)) {
		return BF_EOF;
	}

	bf_stream_t probe = *stream;
	size_t remaining = bit_count;

	while (remaining > 0U) {
		size_t chunk = (remaining > 32U) ? 32U : remaining;
		uint32_t bits = 0U;
		bf_status status = bf_stream_read_bits(&probe, chunk, &bits);
		if (status != BF_OK) {
			return status;
		}
		if (bits != 0U) {
			return BF_EFORMAT;
		}
		remaining -= chunk;
	}

	*stream = probe;
	return BF_OK;
}
