#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool stream_is_valid(const bf_stream_t *stream)
{
	if (stream == NULL) {
		return false;
	}

	if (stream->data == NULL && stream->size > 0) {
		return false;
	}

	if (stream->byte_offset > stream->size) {
		return false;
	}

	if (stream->bit_offset > 7) {
		return false;
	}

	if (stream->byte_offset == stream->size && stream->bit_offset != 0) {
		return false;
	}

	return true;
}

static bool advance_position(size_t size, size_t byte_off, uint8_t bit_off,
    size_t skip_bits, size_t *out_byte, uint8_t *out_bit)
{
	if (byte_off > size || bit_off > 7) {
		return false;
	}

	size_t rem_bytes = size - byte_off;
	uint8_t bits_in_cur_byte =
	    (rem_bytes > 0) ? (uint8_t)(8U - bit_off) : 0U;

	if (skip_bits <= (size_t)bits_in_cur_byte) {
		uint8_t new_bit = (uint8_t)(bit_off + (uint8_t)skip_bits);
		if (new_bit == 8U) {
			*out_byte = byte_off + 1U;
			*out_bit = 0U;
		} else {
			*out_byte = byte_off;
			*out_bit = new_bit;
		}
		return true;
	}

	size_t after_cur_byte = skip_bits - (size_t)bits_in_cur_byte;
	size_t full_bytes_to_skip = after_cur_byte / 8U;
	uint8_t rem_bit_in_byte = (uint8_t)(after_cur_byte % 8U);

	size_t available_after_cur = (rem_bytes > 0) ? (rem_bytes - 1U) : 0U;
	if (full_bytes_to_skip > available_after_cur) {
		return false;
	}

	if (full_bytes_to_skip == available_after_cur &&
	    rem_bit_in_byte != 0U) {
		return false;
	}

	*out_byte = byte_off + ((rem_bytes > 0) ? 1U : 0U) + full_bytes_to_skip;
	*out_bit = rem_bit_in_byte;
	return true;
}

bf_status bf_stream_init(bf_stream_t *stream, const uint8_t *data, size_t size)
{
	if (stream == NULL || (data == NULL && size > 0)) {
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
	size_t next_byte = 0;
	uint8_t next_bit = 0;

	if (!stream_is_valid(stream)) {
		return false;
	}

	if (bit_count == 0) {
		return true;
	}

	return advance_position(stream->size, stream->byte_offset,
	    stream->bit_offset, bit_count, &next_byte, &next_bit);
}

bf_status bf_stream_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value)
{
	if (!stream_is_valid(stream) || out_value == NULL || bit_count == 0 ||
	    bit_count > 32) {
		return BF_EINVAL;
	}

	size_t next_byte = 0;
	uint8_t next_bit = 0;
	if (!advance_position(stream->size, stream->byte_offset,
		stream->bit_offset, bit_count, &next_byte, &next_bit)) {
		return BF_EOF;
	}

	uint32_t result = 0;
	size_t cur_byte = stream->byte_offset;
	uint8_t cur_bit = stream->bit_offset;

	for (size_t i = 0; i < bit_count; i++) {
		uint8_t bit =
		    (uint8_t)((stream->data[cur_byte] >> (7U - cur_bit)) & 1U);
		result = (result << 1) | (uint32_t)bit;

		cur_bit++;
		if (cur_bit == 8U) {
			cur_bit = 0U;
			cur_byte++;
		}
	}

	stream->byte_offset = next_byte;
	stream->bit_offset = next_bit;
	*out_value = result;

	return BF_OK;
}

bf_status bf_stream_read_byte(bf_stream_t *stream, uint8_t *out_value)
{
	uint32_t val = 0;
	bf_status status;

	if (!stream_is_valid(stream) || out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_stream_read_bits(stream, 8, &val);
	if (status != BF_OK) {
		return status;
	}

	*out_value = (uint8_t)val;
	return BF_OK;
}

bf_status bf_stream_read_bytes(
    bf_stream_t *stream, uint8_t *destination, size_t length)
{
	if (!stream_is_valid(stream) || (destination == NULL && length > 0)) {
		return BF_EINVAL;
	}

	if (stream->bit_offset != 0U) {
		return BF_EINVAL;
	}

	if (length == 0) {
		return BF_OK;
	}

	if (length > stream->size - stream->byte_offset) {
		return BF_EOF;
	}

	memmove(destination, stream->data + stream->byte_offset, length);
	stream->byte_offset += length;

	return BF_OK;
}

bf_status bf_stream_read_bool(bf_stream_t *stream, bool *out_value)
{
	uint32_t val = 0;
	bf_status status;

	if (!stream_is_valid(stream) || out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_stream_read_bits(stream, 1, &val);
	if (status != BF_OK) {
		return status;
	}

	*out_value = (val != 0U);
	return BF_OK;
}

bf_status bf_stream_skip_bits(bf_stream_t *stream, size_t bit_count)
{
	size_t next_byte = 0;
	uint8_t next_bit = 0;

	if (!stream_is_valid(stream)) {
		return BF_EINVAL;
	}

	if (bit_count == 0) {
		return BF_OK;
	}

	if (!advance_position(stream->size, stream->byte_offset,
		stream->bit_offset, bit_count, &next_byte, &next_bit)) {
		return BF_EOF;
	}

	stream->byte_offset = next_byte;
	stream->bit_offset = next_bit;

	return BF_OK;
}

bf_status bf_stream_align(bf_stream_t *stream)
{
	if (!stream_is_valid(stream)) {
		return BF_EINVAL;
	}

	if (stream->bit_offset != 0U) {
		if (stream->byte_offset >= stream->size) {
			return BF_EOF;
		}
		stream->byte_offset += 1U;
		stream->bit_offset = 0U;
	}

	return BF_OK;
}

bf_status bf_stream_seek(
    bf_stream_t *stream, size_t byte_offset, uint8_t bit_offset)
{
	if (!stream_is_valid(stream) || bit_offset > 7U) {
		return BF_EINVAL;
	}

	if (byte_offset > stream->size) {
		return BF_EINVAL;
	}

	if (byte_offset == stream->size && bit_offset != 0U) {
		return BF_EINVAL;
	}

	stream->byte_offset = byte_offset;
	stream->bit_offset = bit_offset;

	return BF_OK;
}

bf_status bf_stream_tell(
    const bf_stream_t *stream, size_t *out_byte_offset, uint8_t *out_bit_offset)
{
	if (!stream_is_valid(stream) ||
	    (out_byte_offset == NULL && out_bit_offset == NULL)) {
		return BF_EINVAL;
	}

	if (out_byte_offset != NULL) {
		*out_byte_offset = stream->byte_offset;
	}

	if (out_bit_offset != NULL) {
		*out_bit_offset = stream->bit_offset;
	}

	return BF_OK;
}
