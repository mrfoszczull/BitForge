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
