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
