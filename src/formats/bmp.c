#include <bitforge/formats/bmp.h>
#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

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

static bool bit_count_valid_for_compression(
    uint16_t bit_count, uint32_t compression)
{
	switch (compression) {
	case BF_BMP_COMP_RGB:
		return bit_count == 1U || bit_count == 4U || bit_count == 8U ||
		       bit_count == 16U || bit_count == 24U || bit_count == 32U;
	case BF_BMP_COMP_RLE8:
		return bit_count == 8U;
	case BF_BMP_COMP_RLE4:
		return bit_count == 4U;
	case BF_BMP_COMP_BITFIELDS:
		return bit_count == 16U || bit_count == 32U;
	default:
		return false;
	}
}

bf_status bf_bmp_read_signature(bf_stream_t *stream)
{
	static const uint8_t k_bmp_sig[2] = {'B', 'M'};

	if (!stream_aligned_ready(stream)) {
		return BF_EINVAL;
	}

	return bf_parser_expect_bytes(stream, k_bmp_sig, sizeof(k_bmp_sig));
}

bf_status bf_bmp_read_file_header(
    bf_stream_t *stream, bf_bmp_file_header_t *out_header)
{
	if (!stream_aligned_ready(stream) || out_header == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t probe = *stream;
	bf_bmp_file_header_t hdr;
	bf_status status;

	status = bf_bmp_read_signature(&probe);
	if (status != BF_OK) {
		return status;
	}

	status = bf_parser_read_u32_le(&probe, &hdr.file_size);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&probe, &hdr.reserved1);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&probe, &hdr.reserved2);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&probe, &hdr.pixel_offset);
	if (status != BF_OK) {
		return status;
	}

	if (hdr.pixel_offset <
	    BF_BMP_FILE_HEADER_LEN + BF_BMP_INFO_HEADER_MIN_LEN) {
		return BF_EFORMAT;
	}

	*out_header = hdr;
	*stream = probe;
	return BF_OK;
}

bf_status bf_bmp_read_info_header(
    bf_stream_t *stream, bf_bmp_info_header_t *out_info)
{
	if (!stream_aligned_ready(stream) || out_info == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t probe = *stream;
	size_t start_offset = probe.byte_offset;
	bf_bmp_info_header_t info;
	bf_status status;

	status = bf_parser_read_u32_le(&probe, &info.header_size);
	if (status != BF_OK) {
		return status;
	}

	if (info.header_size < BF_BMP_INFO_HEADER_MIN_LEN) {
		return BF_EFORMAT;
	}

	if ((size_t)info.header_size > probe.size - start_offset) {
		return BF_EOF;
	}

	status = bf_parser_read_i32_le(&probe, &info.width);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_i32_le(&probe, &info.height);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&probe, &info.planes);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&probe, &info.bit_count);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&probe, &info.compression);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&probe, &info.image_size);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_i32_le(&probe, &info.x_pixels_per_meter);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_i32_le(&probe, &info.y_pixels_per_meter);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&probe, &info.colors_used);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&probe, &info.colors_important);
	if (status != BF_OK) {
		return status;
	}

	if (info.width <= 0 || info.height == 0 || info.height == INT32_MIN ||
	    info.planes != 1U ||
	    !bit_count_valid_for_compression(
		info.bit_count, info.compression)) {
		return BF_EFORMAT;
	}

	probe.byte_offset = start_offset + (size_t)info.header_size;

	*out_info = info;
	*stream = probe;
	return BF_OK;
}

bf_status bf_bmp_row_stride(
    uint32_t width, uint16_t bit_count, size_t *out_stride)
{
	if (out_stride == NULL || width == 0U || bit_count == 0U ||
	    bit_count > 64U) {
		return BF_EINVAL;
	}

	uint64_t total_bits = (uint64_t)width * (uint64_t)bit_count;
	if (total_bits > UINT64_MAX - 31ULL) {
		return BF_EOVERFLOW;
	}

	uint64_t dwords = (total_bits + 31ULL) / 32ULL;
	if (dwords > (UINT64_MAX / 4ULL)) {
		return BF_EOVERFLOW;
	}

	uint64_t stride = dwords * 4ULL;
	if (stride > (uint64_t)SIZE_MAX) {
		return BF_EOVERFLOW;
	}

	*out_stride = (size_t)stride;
	return BF_OK;
}

bf_status bf_bmp_get_pixels(bf_stream_t *stream,
    const bf_bmp_file_header_t *file_hdr, const bf_bmp_info_header_t *info_hdr,
    bf_bmp_pixels_t *out_pixels)
{
	if (!stream_aligned_ready(stream) || file_hdr == NULL ||
	    info_hdr == NULL || out_pixels == NULL) {
		return BF_EINVAL;
	}

	if ((size_t)file_hdr->pixel_offset > stream->size) {
		return BF_EOF;
	}

	bool top_down = info_hdr->height < 0;
	uint32_t abs_height = top_down ? (uint32_t)(-(int64_t)info_hdr->height)
				       : (uint32_t)info_hdr->height;

	size_t stride = 0U;
	bf_status status = bf_bmp_row_stride(
	    (uint32_t)info_hdr->width, info_hdr->bit_count, &stride);
	if (status != BF_OK) {
		return status;
	}

	if (stride > 0U && (size_t)abs_height > (SIZE_MAX / stride)) {
		return BF_EOVERFLOW;
	}

	size_t min_pixel_bytes = stride * (size_t)abs_height;
	size_t available = stream->size - (size_t)file_hdr->pixel_offset;

	if ((info_hdr->compression == BF_BMP_COMP_RGB ||
		info_hdr->compression == BF_BMP_COMP_BITFIELDS) &&
	    min_pixel_bytes > available) {
		return BF_EOF;
	}

	status = bf_stream_seek(stream, (size_t)file_hdr->pixel_offset, 0U);
	if (status != BF_OK) {
		return status;
	}

	out_pixels->data = (stream->data != NULL)
			       ? (stream->data + stream->byte_offset)
			       : NULL;
	out_pixels->size = available;
	out_pixels->top_down = top_down;
	out_pixels->abs_height = abs_height;
	return BF_OK;
}
