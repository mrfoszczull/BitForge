#include <bitforge/formats/bmp.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

static void write_u16_le(uint8_t *dst, uint16_t val)
{
	dst[0] = (uint8_t)(val & 0xFFU);
	dst[1] = (uint8_t)((val >> 8) & 0xFFU);
}

static void write_u32_le(uint8_t *dst, uint32_t val)
{
	dst[0] = (uint8_t)(val & 0xFFU);
	dst[1] = (uint8_t)((val >> 8) & 0xFFU);
	dst[2] = (uint8_t)((val >> 16) & 0xFFU);
	dst[3] = (uint8_t)((val >> 24) & 0xFFU);
}

static void write_i32_le(uint8_t *dst, int32_t val)
{
	uint32_t raw = 0U;
	memcpy(&raw, &val, sizeof(raw));
	write_u32_le(dst, raw);
}

static size_t build_valid_24bpp_bmp(
    uint8_t *buf, size_t buf_cap, int32_t width, int32_t height)
{
	size_t stride = 0U;
	uint32_t abs_h =
	    (height < 0) ? (uint32_t)(-(int64_t)height) : (uint32_t)height;
	if (bf_bmp_row_stride((uint32_t)width, 24U, &stride) != BF_OK) {
		return 0U;
	}

	uint32_t pixel_offset =
	    BF_BMP_FILE_HEADER_LEN + BF_BMP_INFO_HEADER_MIN_LEN;
	size_t pixel_bytes = stride * (size_t)abs_h;
	size_t total = (size_t)pixel_offset + pixel_bytes;
	if (total > buf_cap) {
		return 0U;
	}

	memset(buf, 0, total);

	buf[0] = 'B';
	buf[1] = 'M';
	write_u32_le(buf + 2U, (uint32_t)total);
	write_u16_le(buf + 6U, 0U);
	write_u16_le(buf + 8U, 0U);
	write_u32_le(buf + 10U, pixel_offset);

	uint8_t *ih = buf + BF_BMP_FILE_HEADER_LEN;
	write_u32_le(ih + 0U, BF_BMP_INFO_HEADER_MIN_LEN);
	write_i32_le(ih + 4U, width);
	write_i32_le(ih + 8U, height);
	write_u16_le(ih + 12U, 1U);		 /* planes = 1 */
	write_u16_le(ih + 14U, 24U);		 /* bit_count = 24 */
	write_u32_le(ih + 16U, BF_BMP_COMP_RGB); /* BI_RGB */
	write_u32_le(ih + 20U, (uint32_t)pixel_bytes);
	write_i32_le(ih + 24U, 2835);
	write_i32_le(ih + 28U, 2835);
	write_u32_le(ih + 32U, 0U);
	write_u32_le(ih + 36U, 0U);

	for (size_t i = 0; i < pixel_bytes; i++) {
		buf[(size_t)pixel_offset + i] = (uint8_t)(0x10U + (i & 0x7FU));
	}

	return total;
}

static void test_bmp_signature_and_file_header(void)
{
	uint8_t bmp[256] = {0};
	size_t len = build_valid_24bpp_bmp(bmp, sizeof(bmp), 3, 2);
	CHECK(len > BF_BMP_FILE_HEADER_LEN);

	bf_stream_t s;
	CHECK(bf_stream_init(&s, bmp, len) == BF_OK);
	CHECK(bf_bmp_read_signature(&s) == BF_OK);
	CHECK(s.byte_offset == 2U);

	CHECK(bf_stream_init(&s, bmp, len) == BF_OK);
	bf_bmp_file_header_t fhdr;
	memset(&fhdr, 0, sizeof(fhdr));
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_OK);
	CHECK(fhdr.file_size == (uint32_t)len);
	CHECK(fhdr.reserved1 == 0U);
	CHECK(fhdr.reserved2 == 0U);
	CHECK(fhdr.pixel_offset ==
	      BF_BMP_FILE_HEADER_LEN + BF_BMP_INFO_HEADER_MIN_LEN);
	CHECK(s.byte_offset == BF_BMP_FILE_HEADER_LEN);

	uint8_t bad_sig[16] = {'X', 'M'};
	CHECK(bf_stream_init(&s, bad_sig, sizeof(bad_sig)) == BF_OK);
	CHECK(bf_bmp_read_signature(&s) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	uint8_t bad_off[20] = {'B', 'M'};
	write_u32_le(bad_off + 2U, 20U);
	write_u32_le(bad_off + 10U, 14U);
	CHECK(bf_stream_init(&s, bad_off, sizeof(bad_off)) == BF_OK);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	CHECK(bf_stream_init(&s, bmp, 10U) == BF_OK);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_EOF);
	CHECK(s.byte_offset == 0U);

	CHECK(bf_bmp_read_signature(NULL) == BF_EINVAL);
	CHECK(bf_bmp_read_file_header(NULL, &fhdr) == BF_EINVAL);
	CHECK(bf_bmp_read_file_header(&s, NULL) == BF_EINVAL);

	printf("test_bmp_signature_and_file_header finished.\n");
}

static void test_bmp_info_header_and_stride(void)
{
	size_t stride = 0U;
	CHECK(bf_bmp_row_stride(3U, 24U, &stride) == BF_OK && stride == 12U);
	CHECK(bf_bmp_row_stride(1U, 1U, &stride) == BF_OK && stride == 4U);
	CHECK(bf_bmp_row_stride(5U, 8U, &stride) == BF_OK && stride == 8U);

	CHECK(bf_bmp_row_stride(0U, 24U, &stride) == BF_EINVAL);
	CHECK(bf_bmp_row_stride(4U, 0U, &stride) == BF_EINVAL);
	CHECK(bf_bmp_row_stride(4U, 24U, NULL) == BF_EINVAL);

	if (sizeof(size_t) < sizeof(uint64_t)) {
		CHECK(bf_bmp_row_stride(UINT32_MAX, 32U, &stride) ==
		      BF_EOVERFLOW);
	} else {
		CHECK(bf_bmp_row_stride(UINT32_MAX, 32U, &stride) == BF_OK);
	}

	uint8_t bmp[256] = {0};
	size_t len = build_valid_24bpp_bmp(bmp, sizeof(bmp), 3, -2);
	CHECK(len > 0U);

	bf_stream_t s;
	bf_bmp_file_header_t fhdr;
	bf_bmp_info_header_t ihdr;
	CHECK(bf_stream_init(&s, bmp, len) == BF_OK);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_OK);
	CHECK(bf_bmp_read_info_header(&s, &ihdr) == BF_OK);
	CHECK(ihdr.header_size == BF_BMP_INFO_HEADER_MIN_LEN);
	CHECK(ihdr.width == 3);
	CHECK(ihdr.height == -2);
	CHECK(ihdr.planes == 1U);
	CHECK(ihdr.bit_count == 24U);
	CHECK(ihdr.compression == BF_BMP_COMP_RGB);
	CHECK(s.byte_offset == fhdr.pixel_offset);

	bf_bmp_info_header_t huge_info = ihdr;
	huge_info.width = 0x7FFFFFFF;
	huge_info.height = 0x7FFFFFFF;
	huge_info.bit_count = 64U;
	bf_bmp_pixels_t px;
	CHECK(bf_bmp_get_pixels(&s, &fhdr, &huge_info, &px) == BF_EOVERFLOW);

	uint8_t bad_bmp[256] = {0};
	size_t bad_len = build_valid_24bpp_bmp(bad_bmp, sizeof(bad_bmp), 3, 2);
	CHECK(bad_len > 0U);
	write_u16_le(bad_bmp + 26U, 2U);
	CHECK(bf_stream_init(&s, bad_bmp, bad_len) == BF_OK);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_OK);
	size_t pos_before = s.byte_offset;
	CHECK(bf_bmp_read_info_header(&s, &ihdr) == BF_EFORMAT);
	CHECK(s.byte_offset == pos_before);

	build_valid_24bpp_bmp(bad_bmp, sizeof(bad_bmp), 3, 2);
	write_i32_le(bad_bmp + 22U, INT32_MIN);
	CHECK(bf_stream_init(&s, bad_bmp, bad_len) == BF_OK);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_OK);
	CHECK(bf_bmp_read_info_header(&s, &ihdr) == BF_EFORMAT);

	CHECK(bf_bmp_read_info_header(NULL, &ihdr) == BF_EINVAL);
	CHECK(bf_bmp_read_info_header(&s, NULL) == BF_EINVAL);

	printf("test_bmp_info_header_and_stride finished.\n");
}

static void test_bmp_pixels_view(void)
{
	uint8_t bmp[256] = {0};
	size_t len = build_valid_24bpp_bmp(bmp, sizeof(bmp), 3, -2);
	CHECK(len > 0U);

	bf_stream_t s;
	bf_bmp_file_header_t fhdr;
	bf_bmp_info_header_t ihdr;
	bf_bmp_pixels_t px;

	CHECK(bf_stream_init(&s, bmp, len) == BF_OK);
	CHECK(bf_bmp_read_file_header(&s, &fhdr) == BF_OK);
	CHECK(bf_bmp_read_info_header(&s, &ihdr) == BF_OK);
	CHECK(bf_bmp_get_pixels(&s, &fhdr, &ihdr, &px) == BF_OK);

	CHECK(px.data != NULL);
	CHECK(px.data == bmp + fhdr.pixel_offset);
	CHECK(px.size == 24U); /* 2 rows * 12 bytes stride */
	CHECK(px.top_down == true);
	CHECK(px.abs_height == 2U);
	CHECK(s.byte_offset == fhdr.pixel_offset);

	CHECK(
	    bf_stream_init(&s, bmp, (size_t)fhdr.pixel_offset + 10U) == BF_OK);
	CHECK(bf_bmp_get_pixels(&s, &fhdr, &ihdr, &px) == BF_EOF);

	CHECK(bf_bmp_get_pixels(NULL, &fhdr, &ihdr, &px) == BF_EINVAL);
	CHECK(bf_bmp_get_pixels(&s, NULL, &ihdr, &px) == BF_EINVAL);
	CHECK(bf_bmp_get_pixels(&s, &fhdr, NULL, &px) == BF_EINVAL);
	CHECK(bf_bmp_get_pixels(&s, &fhdr, &ihdr, NULL) == BF_EINVAL);

	printf("test_bmp_pixels_view finished.\n");
}

int main(void)
{
	test_bmp_signature_and_file_header();
	test_bmp_info_header_and_stride();
	test_bmp_pixels_view();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All BMP parser tests passed.\n");
	return EXIT_SUCCESS;
}
