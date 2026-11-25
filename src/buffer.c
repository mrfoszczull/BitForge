#include <bitforge/buffer.h>
#include <bitforge/status.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool buffer_is_valid(const bf_buffer_t *buffer)
{
	if (buffer == NULL) {
		return false;
	}

	if (buffer->size > buffer->capacity) {
		return false;
	}

	if (buffer->data == NULL && buffer->capacity > 0) {
		return false;
	}

	return true;
}

static bool view_is_valid(const bf_view_t *view)
{
	if (view == NULL) {
		return false;
	}

	if (view->data == NULL && view->size > 0) {
		return false;
	}

	return true;
}

static bool range_in_bounds(size_t total, size_t offset, size_t length)
{
	if (offset > total || length > total - offset) {
		return false;
	}

	return true;
}

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
	if (!view_is_valid(view) || (destination == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (!range_in_bounds(view->size, offset, length)) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memmove(destination, view->data + offset, length);
	}

	return BF_OK;
}

bf_status bf_view_slice(
    const bf_view_t *view, size_t offset, size_t length, bf_view_t *result)
{
	if (!view_is_valid(view) || result == NULL) {
		return BF_EINVAL;
	}

	if (!range_in_bounds(view->size, offset, length)) {
		return BF_EINVAL;
	}

	result->data = (view->data != NULL) ? (view->data + offset) : NULL;
	result->size = length;

	return BF_OK;
}

bf_status bf_buffer_write(
    bf_buffer_t *buffer, size_t offset, const uint8_t *source, size_t length)
{
	if (!buffer_is_valid(buffer) || (source == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (!range_in_bounds(buffer->capacity, offset, length)) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memmove(buffer->data + offset, source, length);
	}

	return BF_OK;
}

bf_status bf_buffer_get_view(const bf_buffer_t *buffer, bf_view_t *view)
{
	if (!buffer_is_valid(buffer) || view == NULL) {
		return BF_EINVAL;
	}

	view->data = buffer->data;
	view->size = buffer->size;

	return BF_OK;
}

bf_status bf_buffer_append(
    bf_buffer_t *buffer, const uint8_t *source, size_t length)
{
	if (!buffer_is_valid(buffer) || (source == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (length > buffer->capacity - buffer->size) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memmove(buffer->data + buffer->size, source, length);
		buffer->size += length;
	}

	return BF_OK;
}

bf_status bf_buffer_clear(bf_buffer_t *buffer)
{
	if (!buffer_is_valid(buffer)) {
		return BF_EINVAL;
	}

	buffer->size = 0;

	return BF_OK;
}

bf_status bf_buffer_fill(
    bf_buffer_t *buffer, size_t offset, size_t length, uint8_t value)
{
	if (!buffer_is_valid(buffer)) {
		return BF_EINVAL;
	}

	if (!range_in_bounds(buffer->capacity, offset, length)) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memset(buffer->data + offset, value, length);
	}

	return BF_OK;
}

bf_status bf_view_compare(const bf_view_t *a, const bf_view_t *b, bool *equal)
{
	if (!view_is_valid(a) || !view_is_valid(b) || equal == NULL) {
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

	*equal = (memcmp(a->data, b->data, a->size) == 0);

	return BF_OK;
}

bf_status bf_buffer_copy(bf_buffer_t *destination, const bf_buffer_t *source)
{
	if (!buffer_is_valid(destination) || !buffer_is_valid(source)) {
		return BF_EINVAL;
	}

	if (source->size > destination->capacity) {
		return BF_EINVAL;
	}

	if (source->size > 0) {
		memmove(destination->data, source->data, source->size);
	}

	destination->size = source->size;

	return BF_OK;
}

bf_status bf_buffer_read(const bf_buffer_t *buffer, size_t offset,
    uint8_t *destination, size_t length)
{
	if (!buffer_is_valid(buffer) || (destination == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (!range_in_bounds(buffer->size, offset, length)) {
		return BF_EINVAL;
	}

	if (length > 0) {
		memmove(destination, buffer->data + offset, length);
	}

	return BF_OK;
}

bf_status bf_buffer_insert(
    bf_buffer_t *buffer, size_t offset, const uint8_t *source, size_t length)
{
	if (!buffer_is_valid(buffer) || (source == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (offset > buffer->size || length > buffer->capacity - buffer->size) {
		return BF_EINVAL;
	}

	if (length == 0) {
		return BF_OK;
	}

	size_t tail_bytes = buffer->size - offset;

	if (tail_bytes > 0) {
		memmove(buffer->data + offset + length, buffer->data + offset,
		    tail_bytes);
	}

	memmove(buffer->data + offset, source, length);
	buffer->size += length;

	return BF_OK;
}

bf_status bf_buffer_remove(bf_buffer_t *buffer, size_t offset, size_t length)
{
	if (!buffer_is_valid(buffer)) {
		return BF_EINVAL;
	}

	if (!range_in_bounds(buffer->size, offset, length)) {
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
	if (!buffer_is_valid(buffer)) {
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

bool bf_buffer_has_capacity(
    const bf_buffer_t *buffer, size_t additional_capacity)
{
	if (!buffer_is_valid(buffer)) {
		return false;
	}

	return additional_capacity <= (buffer->capacity - buffer->size);
}
