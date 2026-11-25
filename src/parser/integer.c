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

static bf_status read_exact_bytes(
    bf_stream_t *stream, uint8_t *buf, size_t count)
{
	if (!aligned_stream_ready(stream) || buf == NULL || count == 0U) {
		return BF_EINVAL;
	}

	if (count > stream->size - stream->byte_offset) {
		return BF_EOF;
	}

	memcpy(buf, stream->data + stream->byte_offset, count);
	stream->byte_offset += count;
	return BF_OK;
}

static int32_t sign_extend_24(uint32_t raw24)
{
	uint32_t masked = raw24 & 0x00FFFFFFU;
	uint32_t extended =
	    (masked & 0x00800000U) != 0U ? (masked | 0xFF000000U) : masked;
	int32_t result = 0;
	memcpy(&result, &extended, sizeof(result));
	return result;
}

static int64_t u64_to_i64(uint64_t raw)
{
	int64_t result = 0;
	memcpy(&result, &raw, sizeof(result));
	return result;
}

bf_status bf_parser_read_u8(bf_stream_t *stream, uint8_t *out_value)
{
	uint8_t byte = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, &byte, 1U);
	if (status != BF_OK) {
		return status;
	}

	*out_value = byte;
	return BF_OK;
}

bf_status bf_parser_read_u16_le(bf_stream_t *stream, uint16_t *out_value)
{
	uint8_t b[2] = {0U, 0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value = (uint16_t)((uint16_t)b[0] | ((uint16_t)b[1] << 8));
	return BF_OK;
}

bf_status bf_parser_read_u16_be(bf_stream_t *stream, uint16_t *out_value)
{
	uint8_t b[2] = {0U, 0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value = (uint16_t)(((uint16_t)b[0] << 8) | (uint16_t)b[1]);
	return BF_OK;
}

bf_status bf_parser_read_u24_le(bf_stream_t *stream, uint32_t *out_value)
{
	uint8_t b[3] = {0U, 0U, 0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value =
	    (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16);
	return BF_OK;
}

bf_status bf_parser_read_u24_be(bf_stream_t *stream, uint32_t *out_value)
{
	uint8_t b[3] = {0U, 0U, 0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value =
	    ((uint32_t)b[0] << 16) | ((uint32_t)b[1] << 8) | (uint32_t)b[2];
	return BF_OK;
}

bf_status bf_parser_read_u32_le(bf_stream_t *stream, uint32_t *out_value)
{
	uint8_t b[4] = {0U, 0U, 0U, 0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value = (uint32_t)b[0] | ((uint32_t)b[1] << 8) |
		     ((uint32_t)b[2] << 16) | ((uint32_t)b[3] << 24);
	return BF_OK;
}

bf_status bf_parser_read_u32_be(bf_stream_t *stream, uint32_t *out_value)
{
	uint8_t b[4] = {0U, 0U, 0U, 0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value = ((uint32_t)b[0] << 24) | ((uint32_t)b[1] << 16) |
		     ((uint32_t)b[2] << 8) | (uint32_t)b[3];
	return BF_OK;
}

bf_status bf_parser_read_u64_le(bf_stream_t *stream, uint64_t *out_value)
{
	uint8_t b[8] = {0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value = (uint64_t)b[0] | ((uint64_t)b[1] << 8) |
		     ((uint64_t)b[2] << 16) | ((uint64_t)b[3] << 24) |
		     ((uint64_t)b[4] << 32) | ((uint64_t)b[5] << 40) |
		     ((uint64_t)b[6] << 48) | ((uint64_t)b[7] << 56);
	return BF_OK;
}

bf_status bf_parser_read_u64_be(bf_stream_t *stream, uint64_t *out_value)
{
	uint8_t b[8] = {0U};
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = read_exact_bytes(stream, b, sizeof(b));
	if (status != BF_OK) {
		return status;
	}

	*out_value = ((uint64_t)b[0] << 56) | ((uint64_t)b[1] << 48) |
		     ((uint64_t)b[2] << 40) | ((uint64_t)b[3] << 32) |
		     ((uint64_t)b[4] << 24) | ((uint64_t)b[5] << 16) |
		     ((uint64_t)b[6] << 8) | (uint64_t)b[7];
	return BF_OK;
}

bf_status bf_parser_read_i8(bf_stream_t *stream, int8_t *out_value)
{
	uint8_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u8(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(out_value, &raw, sizeof(*out_value));
	return BF_OK;
}

bf_status bf_parser_read_i16_le(bf_stream_t *stream, int16_t *out_value)
{
	uint16_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u16_le(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(out_value, &raw, sizeof(*out_value));
	return BF_OK;
}

bf_status bf_parser_read_i16_be(bf_stream_t *stream, int16_t *out_value)
{
	uint16_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u16_be(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(out_value, &raw, sizeof(*out_value));
	return BF_OK;
}

bf_status bf_parser_read_i24_le(bf_stream_t *stream, int32_t *out_value)
{
	uint32_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u24_le(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	*out_value = sign_extend_24(raw);
	return BF_OK;
}

bf_status bf_parser_read_i24_be(bf_stream_t *stream, int32_t *out_value)
{
	uint32_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u24_be(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	*out_value = sign_extend_24(raw);
	return BF_OK;
}

bf_status bf_parser_read_i32_le(bf_stream_t *stream, int32_t *out_value)
{
	uint32_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u32_le(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(out_value, &raw, sizeof(*out_value));
	return BF_OK;
}

bf_status bf_parser_read_i32_be(bf_stream_t *stream, int32_t *out_value)
{
	uint32_t raw = 0U;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u32_be(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	memcpy(out_value, &raw, sizeof(*out_value));
	return BF_OK;
}

bf_status bf_parser_read_i64_le(bf_stream_t *stream, int64_t *out_value)
{
	uint64_t raw = 0ULL;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u64_le(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	*out_value = u64_to_i64(raw);
	return BF_OK;
}

bf_status bf_parser_read_i64_be(bf_stream_t *stream, int64_t *out_value)
{
	uint64_t raw = 0ULL;
	bf_status status;

	if (out_value == NULL) {
		return BF_EINVAL;
	}

	status = bf_parser_read_u64_be(stream, &raw);
	if (status != BF_OK) {
		return status;
	}

	*out_value = u64_to_i64(raw);
	return BF_OK;
}
