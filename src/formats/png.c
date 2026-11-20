#include <bitforge/formats/png.h>
#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static const uint8_t k_png_sig[BF_PNG_SIG_LEN] = {
    0x89U, 'P', 'N', 'G', '\r', '\n', 0x1AU, '\n'};

static bool stream_aligned_ready(const bf_stream_t *stream)
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

static bool ihdr_combination_valid(uint8_t bit_depth, uint8_t color_type,
    uint8_t compression, uint8_t filter, uint8_t interlace)
{
	if (compression != 0U || filter != 0U || interlace > 1U) {
		return false;
	}

	switch (color_type) {
	case 0U: /* Grayscale */
		return bit_depth == 1U || bit_depth == 2U || bit_depth == 4U ||
		       bit_depth == 8U || bit_depth == 16U;
	case 2U: /* RGB */
	case 4U: /* Grayscale + Alpha */
	case 6U: /* RGBA */
		return bit_depth == 8U || bit_depth == 16U;
	case 3U: /* Indexed */
		return bit_depth == 1U || bit_depth == 2U || bit_depth == 4U ||
		       bit_depth == 8U;
	default:
		return false;
	}
}

uint32_t bf_png_crc32(const uint8_t *data, size_t length)
{
	uint32_t crc = 0xFFFFFFFFU;

	if (data == NULL || length == 0U) {
		return crc ^ 0xFFFFFFFFU;
	}

	for (size_t i = 0; i < length; i++) {
		crc ^= (uint32_t)data[i];
		for (unsigned int bit = 0; bit < 8U; bit++) {
			uint32_t lsb = crc & 1U;
			crc >>= 1;
			if (lsb != 0U) {
				crc ^= 0xEDB88320U;
			}
		}
	}

	return crc ^ 0xFFFFFFFFU;
}

bool bf_png_chunk_is(const bf_png_chunk_t *chunk, const char four_cc[4])
{
	if (chunk == NULL || four_cc == NULL) {
		return false;
	}
	return memcmp(chunk->type, four_cc, BF_PNG_CHUNK_TYPE_LEN) == 0;
}

bf_status bf_png_verify_chunk_crc(const bf_png_chunk_t *chunk)
{
	if (chunk == NULL) {
		return BF_EINVAL;
	}
	if (chunk->length > 0U && chunk->data == NULL) {
		return BF_EINVAL;
	}

	/* CRC covers the 4-byte chunk type followed by chunk payload bytes */
	uint32_t crc = 0xFFFFFFFFU;
	const uint8_t *type_bytes = (const uint8_t *)chunk->type;

	for (size_t i = 0; i < BF_PNG_CHUNK_TYPE_LEN; i++) {
		crc ^= (uint32_t)type_bytes[i];
		for (unsigned int bit = 0; bit < 8U; bit++) {
			uint32_t lsb = crc & 1U;
			crc >>= 1;
			if (lsb != 0U) {
				crc ^= 0xEDB88320U;
			}
		}
	}

	for (size_t i = 0; i < (size_t)chunk->length; i++) {
		crc ^= (uint32_t)chunk->data[i];
		for (unsigned int bit = 0; bit < 8U; bit++) {
			uint32_t lsb = crc & 1U;
			crc >>= 1;
			if (lsb != 0U) {
				crc ^= 0xEDB88320U;
			}
		}
	}

	uint32_t computed = crc ^ 0xFFFFFFFFU;
	return (computed == chunk->crc) ? BF_OK : BF_EFORMAT;
}

bf_status bf_png_read_signature(bf_stream_t *stream)
{
	if (!stream_aligned_ready(stream)) {
		return BF_EINVAL;
	}

	return bf_parser_expect_bytes(stream, k_png_sig, BF_PNG_SIG_LEN);
}

bf_status bf_png_read_chunk(bf_stream_t *stream, bf_png_chunk_t *out_chunk)
{
	if (!stream_aligned_ready(stream) || out_chunk == NULL) {
		return BF_EINVAL;
	}

	if (stream->byte_offset == stream->size) {
		return BF_EOF;
	}

	bf_stream_t probe = *stream;
	uint32_t length = 0U;
	uint8_t type_buf[BF_PNG_CHUNK_TYPE_LEN] = {0U};
	uint32_t crc = 0U;
	bf_status status;

	status = bf_parser_read_u32_be(&probe, &length);
	if (status != BF_OK) {
		return (probe.byte_offset == stream->byte_offset) ? BF_EOF
								  : status;
	}

	/* PNG chunk length is a signed 31-bit non-negative integer in spec */
	if (length > 0x7FFFFFFFU) {
		return BF_EFORMAT;
	}

	status = bf_parser_expect_bytes(&probe, type_buf, 0U);
	(void)status;
	if ((size_t)length > probe.size - probe.byte_offset) {
		return BF_EOF;
	}

	/* Read 4-byte chunk type */
	if (BF_PNG_CHUNK_TYPE_LEN > probe.size - probe.byte_offset) {
		return BF_EOF;
	}
	memcpy(type_buf, probe.data + probe.byte_offset, BF_PNG_CHUNK_TYPE_LEN);
	probe.byte_offset += BF_PNG_CHUNK_TYPE_LEN;

	const uint8_t *payload = NULL;
	if (length > 0U) {
		if ((size_t)length > probe.size - probe.byte_offset) {
			return BF_EOF;
		}
		payload = probe.data + probe.byte_offset;
		probe.byte_offset += (size_t)length;
	}

	status = bf_parser_read_u32_be(&probe, &crc);
	if (status != BF_OK) {
		return status;
	}

	out_chunk->length = length;
	memcpy(out_chunk->type, type_buf, BF_PNG_CHUNK_TYPE_LEN);
	out_chunk->type[BF_PNG_CHUNK_TYPE_LEN] = '\0';
	out_chunk->data = payload;
	out_chunk->crc = crc;

	*stream = probe;
	return BF_OK;
}

bf_status bf_png_read_ihdr(bf_stream_t *stream, bf_png_ihdr_t *out_ihdr)
{
	if (!stream_aligned_ready(stream) || out_ihdr == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t probe = *stream;
	bf_png_chunk_t chunk;
	bf_status status = bf_png_read_chunk(&probe, &chunk);
	if (status != BF_OK) {
		return status;
	}

	if (!bf_png_chunk_is(&chunk, "IHDR") ||
	    chunk.length != BF_PNG_IHDR_DATA_LEN) {
		return BF_EFORMAT;
	}

	status = bf_png_verify_chunk_crc(&chunk);
	if (status != BF_OK) {
		return status;
	}

	bf_stream_t payload_stream;
	status = bf_stream_init(&payload_stream, chunk.data, chunk.length);
	if (status != BF_OK) {
		return status;
	}

	bf_png_ihdr_t ihdr;
	status = bf_parser_read_u32_be(&payload_stream, &ihdr.width);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_be(&payload_stream, &ihdr.height);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u8(&payload_stream, &ihdr.bit_depth);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u8(&payload_stream, &ihdr.color_type);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u8(&payload_stream, &ihdr.compression);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u8(&payload_stream, &ihdr.filter);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u8(&payload_stream, &ihdr.interlace);
	if (status != BF_OK) {
		return status;
	}

	if (ihdr.width == 0U || ihdr.height == 0U ||
	    !ihdr_combination_valid(ihdr.bit_depth, ihdr.color_type,
		ihdr.compression, ihdr.filter, ihdr.interlace)) {
		return BF_EFORMAT;
	}

	*out_ihdr = ihdr;
	*stream = probe;
	return BF_OK;
}
