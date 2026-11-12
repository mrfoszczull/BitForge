#include <bitforge/status.h>
#include <bitforge/buffer.h>
#include <string.h>

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

bf_status bf_view_read(
    const bf_view_t *view, size_t offset, uint8_t *destination, size_t length)
{
	if (view == NULL || (destination == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (view->data == NULL && view->size > 0) {
		return BF_EINVAL;
	}

	if (offset > view->size || length > view->size - offset) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memcpy(destination, view->data + offset, length);
	}

	return BF_OK;
}

bf_status bf_view_slice(
    const bf_view_t *view, size_t offset, size_t length, bf_view_t *result)
{
	if (view == NULL || result == NULL) {
		return BF_EINVAL;
	}

	if (view->data == NULL && view->size > 0) {
		return BF_EINVAL;
	}

	if (offset > view->size || length > view->size - offset) {
		return BF_EINVAL;
	}

	result->data = view->data != NULL ? view->data + offset : NULL;
	result->size = length;

	return BF_OK;
}

bf_status bf_buffer_write(
    bf_buffer_t *buffer, size_t offset, const uint8_t *source, size_t length)
{
	if (buffer == NULL || (source == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (offset > buffer->capacity || length > buffer->capacity - offset) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memcpy(buffer->data + offset, source, length);
	}

	return BF_OK;
}

bf_status bf_buffer_get_view(const bf_buffer_t *buffer, bf_view_t *view)
{
	if (buffer == NULL || view == NULL) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	view->data = buffer->data;
	view->size = buffer->size;

	return BF_OK;
}

bf_status bf_buffer_append(
    bf_buffer_t *buffer, const uint8_t *source, size_t length)
{
	if (buffer == NULL || (source == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (length > buffer->capacity - buffer->size) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memcpy(buffer->data + buffer->size, source, length);
	}

	buffer->size += length;

	return BF_OK;
}
