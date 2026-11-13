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

bf_status bf_buffer_clear(bf_buffer_t *buffer)
{
	if (buffer == NULL) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	buffer->size = 0;

	return BF_OK;
}

bf_status bf_buffer_fill(
    bf_buffer_t *buffer, size_t offset, size_t length, uint8_t value)
{
	if (buffer == NULL) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (offset > buffer->capacity || length > buffer->capacity - offset) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memset(buffer->data + offset, value, length);
	}

	return BF_OK;
}

bf_status bf_view_compare(const bf_view_t *a, const bf_view_t *b, bool *equal)
{
	if (a == NULL || b == NULL || equal == NULL) {
		return BF_EINVAL;
	}

	if ((a->data == NULL && a->size > 0) ||
	    (b->data == NULL && b->size > 0)) {
		return BF_EINVAL;
	}

	if (a->size != b->size) {
		*equal = false;
		return BF_OK;
	}

	if (a->size == 0) {
		*equal = true;
		return BF_OK;
	}

	*equal = memcmp(a->data, b->data, a->size) == 0;

	return BF_OK;
}

bf_status bf_buffer_copy(bf_buffer_t *destination, const bf_buffer_t *source)
{
	if (destination == NULL || source == NULL) {
		return BF_EINVAL;
	}

	if (source->size > source->capacity ||
	    destination->size > destination->capacity) {
		return BF_EINVAL;
	}

	if ((source->data == NULL && source->capacity > 0) ||
	    (destination->data == NULL && destination->capacity > 0)) {
		return BF_EINVAL;
	}

	if (source->size > destination->capacity) {
		return BF_EINVAL;
	}

	if (source->size > 0) {
		memcpy(destination->data, source->data, source->size);
	}

	destination->size = source->size;

	return BF_OK;
}

bf_status bf_buffer_read(const bf_buffer_t *buffer, size_t offset,
    uint8_t *destination, size_t length)
{
	if (buffer == NULL || (destination == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (offset > buffer->size || length > buffer->size - offset) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memcpy(destination, buffer->data + offset, length);
	}

	return BF_OK;
}

bf_status bf_buffer_insert(
    bf_buffer_t *buffer, size_t offset, const uint8_t *source, size_t length)
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

	if (offset > buffer->size) {
		return BF_EINVAL;
	}

	if (length > buffer->capacity - buffer->size) {
		return BF_EINVAL;
	}

	size_t tail_bytes = buffer->size - offset;

	if (tail_bytes > 0) {
		memmove(buffer->data + offset + length, buffer->data + offset,
		    tail_bytes);
	}

	if (length > 0) {
		memcpy(buffer->data + offset, source, length);
	}

	buffer->size += length;

	return BF_OK;
}

bf_status bf_buffer_remove(bf_buffer_t *buffer, size_t offset, size_t length)
{
	if (buffer == NULL) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (offset > buffer->size || length > buffer->size - offset) {
		return BF_EINVAL;
	}

	if (length == 0) {
		return BF_OK;
	}

	size_t tail_offset = offset + length;
	size_t tail_bytes = buffer->size - tail_offset;

	if (tail_bytes > 0) {
		memmove(buffer->data + offset, buffer->data + tail_offset,
		    tail_bytes);
	}

	buffer->size -= length;

	return BF_OK;
}

bf_status bf_buffer_resize(bf_buffer_t *buffer, size_t new_size)
{
	if (buffer == NULL) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (new_size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (new_size > buffer->size) {
		memset(buffer->data + buffer->size, 0, new_size - buffer->size);
	}

	buffer->size = new_size;

	return BF_OK;
}

bf_status bf_buffer_reserve(
    const bf_buffer_t *buffer, size_t additional_capacity)
{
	if (buffer == NULL) {
		return BF_EINVAL;
	}

	if (buffer->size > buffer->capacity) {
		return BF_EINVAL;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return BF_EINVAL;
	}

	if (additional_capacity > buffer->capacity - buffer->size) {
		return BF_EINVAL;
	}

	return BF_OK;
}
