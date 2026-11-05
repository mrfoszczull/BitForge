#include <bitforge/stream.h>
#include <bitforge/status.h>

bf_status bf_stream_init(bf_stream_t *stream, const uint8_t *data, size_t size)
{
	if (stream == NULL || data == NULL) {
		return BF_EINVAL;
	}

	stream->data = data;
	stream->size = size;
	stream->byte_offset = 0;
	stream->bit_offset = 0;

	return BF_OK;
}

bool bf_stream_has_remaining(const bf_stream_t *stream, size_t bit_count)
{
	if (stream == NULL) {
		return false;
	}

	size_t actual_position = (stream->byte_offset * 8) + stream->bit_offset;
	size_t total_bits = stream->size * 8;

	return (actual_position + bit_count <= total_bits);
}

bf_status bf_stream_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value)
{
	if (stream == NULL || out_value == NULL) {
		return BF_EINVAL;
	}

	if (bit_count == 0) {
		*out_value = 0;
		return BF_OK;
	}

	if (!bf_stream_has_remaining(stream, bit_count)) {
		return BF_EIO;
	}

	uint32_t result = 0;

	for (size_t i = 0; i < bit_count; i++) {
		uint8_t current_byte = stream->data[stream->byte_offset];
		uint8_t bit = (current_byte >> (7 - stream->bit_offset)) & 1;

		result = (result << 1) | bit;

		stream->bit_offset++;
		if (stream->bit_offset == 8) {
			stream->bit_offset = 0;
			stream->byte_offset++;
		}
	}

	*out_value = result;
	return BF_OK;
}

bf_status bf_stream_skip_bits(bf_stream_t *stream, size_t bit_count)
{
	if (stream == NULL) {
		return BF_EINVAL;
	}

	if (bit_count == 0) {
		return BF_OK;
	}

	if (!bf_stream_has_remaining(stream, bit_count)) {
		return BF_EIO;
	}

	size_t current_bits = (stream->byte_offset * 8) + stream->bit_offset;
	size_t target_bits = current_bits + bit_count;

	stream->byte_offset = target_bits / 8;
	stream->bit_offset = target_bits % 8;

	return BF_OK;
}
