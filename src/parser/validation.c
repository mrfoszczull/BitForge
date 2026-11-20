#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool aligned_stream_ready(const bf_stream_t *stream)
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
	if (stream->bit_offset != 0U) {
		return false;
	}
	return true;
}

bf_status bf_parser_expect_bytes(
    bf_stream_t *stream, const uint8_t *expected, size_t length)
{
	if (!aligned_stream_ready(stream) ||
	    (expected == NULL && length > 0U)) {
		return BF_EINVAL;
	}

	if (length == 0U) {
		return BF_OK;
	}

	if (length > stream->size - stream->byte_offset) {
		return BF_EOF;
	}

	if (memcmp(stream->data + stream->byte_offset, expected, length) != 0) {
		return BF_EFORMAT;
	}

	stream->byte_offset += length;
	return BF_OK;
}

bf_status bf_parser_expect_u32_be(bf_stream_t *stream, uint32_t expected)
{
	bf_stream_t probe;
	uint32_t actual = 0U;
	bf_status status;

	if (!aligned_stream_ready(stream)) {
		return BF_EINVAL;
	}

	probe = *stream;
	status = bf_parser_read_u32_be(&probe, &actual);
	if (status != BF_OK) {
		return status;
	}

	if (actual != expected) {
		return BF_EFORMAT;
	}

	*stream = probe;
	return BF_OK;
}

bf_status bf_parser_expect_u32_le(bf_stream_t *stream, uint32_t expected)
{
	bf_stream_t probe;
	uint32_t actual = 0U;
	bf_status status;

	if (!aligned_stream_ready(stream)) {
		return BF_EINVAL;
	}

	probe = *stream;
	status = bf_parser_read_u32_le(&probe, &actual);
	if (status != BF_OK) {
		return status;
	}

	if (actual != expected) {
		return BF_EFORMAT;
	}

	*stream = probe;
	return BF_OK;
}

bf_status bf_parser_require_bytes(
    const bf_stream_t *stream, size_t required_bytes)
{
	if (!aligned_stream_ready(stream)) {
		return BF_EINVAL;
	}

	if (required_bytes == 0U) {
		return BF_OK;
	}

	if (required_bytes > stream->size - stream->byte_offset) {
		return BF_EOF;
	}

	return BF_OK;
}
