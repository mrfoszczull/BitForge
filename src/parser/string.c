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

static bool string_out_ready(const char *out_buf, size_t buf_size)
{
	return out_buf != NULL && buf_size > 0U;
}

bf_status bf_parser_read_f32_le(bf_stream_t *stream, float *out_value)
{
	uint32_t raw = 0U;
	float val = 0.0f;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u32_le(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(&val, &raw, sizeof(val));
	*out_value = val;
	return BF_OK;
}

bf_status bf_parser_read_f32_be(bf_stream_t *stream, float *out_value)
{
	uint32_t raw = 0U;
	float val = 0.0f;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u32_be(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(&val, &raw, sizeof(val));
	*out_value = val;
	return BF_OK;
}

bf_status bf_parser_read_f64_le(bf_stream_t *stream, double *out_value)
{
	uint64_t raw = 0ULL;
	double val = 0.0;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u64_le(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(&val, &raw, sizeof(val));
	*out_value = val;
	return BF_OK;
}

bf_status bf_parser_read_f64_be(bf_stream_t *stream, double *out_value)
{
	uint64_t raw = 0ULL;
	double val = 0.0;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u64_be(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(&val, &raw, sizeof(val));
	*out_value = val;
	return BF_OK;
}

bf_status bf_parser_read_fixed_string(
    bf_stream_t *stream, size_t length, char *out_buf, size_t buf_size)
{
	if (!aligned_stream_ready(stream) ||
	    !string_out_ready(out_buf, buf_size)) {
		return BF_EINVAL;
	}

	if (length >= buf_size) {
		return BF_EOVERFLOW;
	}

	if (length > stream->size - stream->byte_offset) {
		return BF_EOF;
	}

	if (length > 0U) {
		memcpy(out_buf, stream->data + stream->byte_offset, length);
		stream->byte_offset += length;
	}
	out_buf[length] = '\0';

	return BF_OK;
}

bf_status bf_parser_read_cstr(
    bf_stream_t *stream, char *out_buf, size_t buf_size)
{
	if (!aligned_stream_ready(stream) ||
	    !string_out_ready(out_buf, buf_size)) {
		return BF_EINVAL;
	}

	size_t remaining = stream->size - stream->byte_offset;
	const uint8_t *start = stream->data + stream->byte_offset;
	const uint8_t *nul_pos = NULL;

	for (size_t i = 0; i < remaining; i++) {
		if (start[i] == 0U) {
			nul_pos = start + i;
			break;
		}
	}

	if (nul_pos == NULL) {
		return BF_EFORMAT;
	}

	size_t str_len = (size_t)(nul_pos - start);
	if (str_len >= buf_size) {
		return BF_EOVERFLOW;
	}

	if (str_len > 0U) {
		memcpy(out_buf, start, str_len);
	}
	out_buf[str_len] = '\0';

	stream->byte_offset += str_len + 1U;
	return BF_OK;
}

bf_status bf_parser_read_pascal_u8_string(
    bf_stream_t *stream, char *out_buf, size_t buf_size)
{
	if (!aligned_stream_ready(stream) ||
	    !string_out_ready(out_buf, buf_size)) {
		return BF_EINVAL;
	}

	if (stream->byte_offset >= stream->size) {
		return BF_EOF;
	}

	size_t str_len = (size_t)stream->data[stream->byte_offset];
	if (str_len >= buf_size) {
		return BF_EOVERFLOW;
	}

	size_t available_after_len = stream->size - stream->byte_offset - 1U;
	if (str_len > available_after_len) {
		return BF_EOF;
	}

	if (str_len > 0U) {
		memcpy(
		    out_buf, stream->data + stream->byte_offset + 1U, str_len);
	}
	out_buf[str_len] = '\0';

	stream->byte_offset += 1U + str_len;
	return BF_OK;
}
