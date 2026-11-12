#include <bitforge/buffer.h>

bf_status bf_view_init(bf_view_t *view, const uint8_t *data, size_t size)
{
	if (view == NULL || (data == NULL && size > 0)) {
		return BF_EINVAL;
	}

	view->data = data;
	view->size = size;

	return BF_OK;
}

bf_status bf_buffer_init(bf_buffer_t *buffer, uint8_t *data, size_t capacity)
{
	if (buffer == NULL || (data == NULL && capacity > 0)) {
		return BF_EINVAL;
	}

	buffer->data = data;
	buffer->capacity = capacity;
	buffer->size = 0;

	return BF_OK;
}
