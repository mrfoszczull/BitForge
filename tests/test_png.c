#include <bitforge/formats/png.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

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

static void write_u32_be(uint8_t *dst, uint32_t val)
{
	dst[0] = (uint8_t)((val >> 24) & 0xFFU);
	dst[1] = (uint8_t)((val >> 16) & 0xFFU);
	dst[2] = (uint8_t)((val >> 8) & 0xFFU);
	dst[3] = (uint8_t)(val & 0xFFU);
}

static size_t append_chunk(uint8_t *buf, size_t buf_cap, size_t offset,
    const char four_cc[4], const uint8_t *payload, uint32_t payload_len)
{
	size_t total =
	    BF_PNG_CHUNK_TYPE_LEN + (size_t)payload_len + BF_PNG_CHUNK_CRC_LEN;
	if (offset + 4U + total > buf_cap) {
		return offset;
	}

	write_u32_be(buf + offset, payload_len);
	memcpy(buf + offset + 4U, four_cc, BF_PNG_CHUNK_TYPE_LEN);
	if (payload_len > 0U && payload != NULL) {
		memcpy(buf + offset + 4U + BF_PNG_CHUNK_TYPE_LEN, payload,
		    payload_len);
	}

	/* Compute CRC-32 over type + payload */
	uint32_t crc = 0xFFFFFFFFU;
	const uint8_t *crc_input = buf + offset + 4U;
	size_t crc_len = BF_PNG_CHUNK_TYPE_LEN + (size_t)payload_len;
	for (size_t i = 0; i < crc_len; i++) {
		crc ^= (uint32_t)crc_input[i];
		for (unsigned int bit = 0; bit < 8U; bit++) {
			uint32_t lsb = crc & 1U;
			crc >>= 1;
			if (lsb != 0U) {
				crc ^= 0xEDB88320U;
			}
		}
	}
	crc ^= 0xFFFFFFFFU;

	write_u32_be(buf + offset + 4U + crc_len, crc);
	return offset + 4U + total;
}

static size_t build_valid_minimal_png(uint8_t *buf, size_t buf_cap)
{
	static const uint8_t sig[BF_PNG_SIG_LEN] = {
	    0x89U, 'P', 'N', 'G', '\r', '\n', 0x1AU, '\n'};
	if (buf_cap < BF_PNG_SIG_LEN) {
		return 0U;
	}

	memcpy(buf, sig, BF_PNG_SIG_LEN);
	size_t pos = BF_PNG_SIG_LEN;

	/* IHDR: 16x8, bit_depth=8, color_type=2 (RGB), comp=0, filter=0,
	 * interlace=0 */
	uint8_t ihdr[BF_PNG_IHDR_DATA_LEN] = {
	    0x00U, 0x00U, 0x00U, 0x10U, /* width = 16 */
	    0x00U, 0x00U, 0x00U, 0x08U, /* height = 8 */
	    0x08U,			/* bit_depth = 8 */
	    0x02U,			/* color_type = 2 (RGB) */
	    0x00U,			/* compression = 0 */
	    0x00U,			/* filter = 0 */
	    0x00U			/* interlace = 0 */
	};
	pos = append_chunk(buf, buf_cap, pos, "IHDR", ihdr, sizeof(ihdr));

	/* Sample IDAT payload */
	uint8_t idat[4] = {0x78U, 0x9CU, 0x01U, 0x00U};
	pos = append_chunk(buf, buf_cap, pos, "IDAT", idat, sizeof(idat));

	/* Empty IEND chunk */
	pos = append_chunk(buf, buf_cap, pos, "IEND", NULL, 0U);

	return pos;
}

static void test_png_signature_and_errors(void)
{
	uint8_t png[128] = {0};
	size_t len = build_valid_minimal_png(png, sizeof(png));
	CHECK(len > BF_PNG_SIG_LEN);

	bf_stream_t s;
	CHECK(bf_stream_init(&s, png, len) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_OK);
	CHECK(s.byte_offset == BF_PNG_SIG_LEN);

	/* Bad signature -> BF_EFORMAT, offset stays at 0 */
	uint8_t bad[8] = {'N', 'O', 'T', '_', 'P', 'N', 'G', '!'};
	CHECK(bf_stream_init(&s, bad, sizeof(bad)) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	/* Truncated signature -> BF_EOF */
	CHECK(bf_stream_init(&s, png, 4U) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_EOF);
	CHECK(s.byte_offset == 0U);

	/* Invalid / unaligned stream -> BF_EINVAL */
	CHECK(bf_png_read_signature(NULL) == BF_EINVAL);
	CHECK(bf_stream_init(&s, png, len) == BF_OK);
	CHECK(bf_stream_seek(&s, 0U, 1U) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_EINVAL);

	printf("test_png_signature_and_errors finished.\n");
}

static void test_png_ihdr_and_validation(void)
{
	uint8_t png[128] = {0};
	size_t len = build_valid_minimal_png(png, sizeof(png));
	CHECK(len > 0U);

	bf_stream_t s;
	CHECK(bf_stream_init(&s, png, len) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_OK);

	bf_png_ihdr_t ihdr;
	memset(&ihdr, 0, sizeof(ihdr));
	CHECK(bf_png_read_ihdr(&s, &ihdr) == BF_OK);
	CHECK(ihdr.width == 16U);
	CHECK(ihdr.height == 8U);
	CHECK(ihdr.bit_depth == 8U);
	CHECK(ihdr.color_type == 2U);
	CHECK(ihdr.compression == 0U);
	CHECK(ihdr.filter == 0U);
	CHECK(ihdr.interlace == 0U);
	CHECK(s.byte_offset ==
	      BF_PNG_SIG_LEN + 4U + 4U + BF_PNG_IHDR_DATA_LEN + 4U);

	/* Corrupt last byte (inside IHDR CRC) -> BF_EFORMAT, offset preserved
	 */
	uint8_t bad_crc[128] = {0};
	memcpy(bad_crc, png, len);
	size_t ihdr_end = BF_PNG_SIG_LEN + 4U + 4U + BF_PNG_IHDR_DATA_LEN + 4U;
	bad_crc[ihdr_end - 1U] ^= 0xFFU;

	CHECK(bf_stream_init(&s, bad_crc, len) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_OK);
	size_t pos_before = s.byte_offset;
	CHECK(bf_png_read_ihdr(&s, &ihdr) == BF_EFORMAT);
	CHECK(s.byte_offset == pos_before);

	/* Invalid color_type / bit_depth combination (RGB with bit_depth=4) */
	uint8_t bad_ihdr_buf[64] = {0};
	static const uint8_t sig[BF_PNG_SIG_LEN] = {
	    0x89U, 'P', 'N', 'G', '\r', '\n', 0x1AU, '\n'};
	memcpy(bad_ihdr_buf, sig, BF_PNG_SIG_LEN);
	uint8_t bad_ihdr_payload[BF_PNG_IHDR_DATA_LEN] = {0x00U, 0x00U, 0x00U,
	    0x04U, 0x00U, 0x00U, 0x00U, 0x04U,
	    0x04U, /* invalid bit_depth for color_type 2 */
	    0x02U, 0x00U, 0x00U, 0x00U};
	size_t bad_len = append_chunk(bad_ihdr_buf, sizeof(bad_ihdr_buf),
	    BF_PNG_SIG_LEN, "IHDR", bad_ihdr_payload, sizeof(bad_ihdr_payload));
	CHECK(bf_stream_init(&s, bad_ihdr_buf, bad_len) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_OK);
	CHECK(bf_png_read_ihdr(&s, &ihdr) == BF_EFORMAT);

	/* NULL arguments */
	CHECK(bf_png_read_ihdr(NULL, &ihdr) == BF_EINVAL);
	CHECK(bf_png_read_ihdr(&s, NULL) == BF_EINVAL);

	printf("test_png_ihdr_and_validation finished.\n");
}

static void test_png_chunks_and_crc32(void)
{
	/* Known IEEE 802.3 / PNG CRC-32 check value for "123456789" is
	 * 0xCBF43926U */
	static const uint8_t check_str[] = "123456789";
	CHECK(bf_png_crc32(check_str, 9U) == 0xCBF43926U);

	uint8_t png[128] = {0};
	size_t len = build_valid_minimal_png(png, sizeof(png));
	CHECK(len > 0U);

	bf_stream_t s;
	CHECK(bf_stream_init(&s, png, len) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_OK);

	bf_png_chunk_t chunk;
	int chunk_count = 0;
	bool saw_ihdr = false;
	bool saw_idat = false;
	bool saw_iend = false;

	for (;;) {
		bf_status st = bf_png_read_chunk(&s, &chunk);
		if (st == BF_EOF) {
			break;
		}
		CHECK(st == BF_OK);
		CHECK(bf_png_verify_chunk_crc(&chunk) == BF_OK);
		chunk_count++;

		if (bf_png_chunk_is(&chunk, "IHDR")) {
			saw_ihdr = true;
			CHECK(chunk.length == BF_PNG_IHDR_DATA_LEN);
		} else if (bf_png_chunk_is(&chunk, "IDAT")) {
			saw_idat = true;
			CHECK(chunk.length == 4U);
			CHECK(chunk.data != NULL);
		} else if (bf_png_chunk_is(&chunk, "IEND")) {
			saw_iend = true;
			CHECK(chunk.length == 0U);
		}
	}

	CHECK(chunk_count == 3);
	CHECK(saw_ihdr && saw_idat && saw_iend);

	/* Truncated chunk payload -> BF_EOF without advancing stream */
	CHECK(bf_stream_init(&s, png, len - 2U) == BF_OK);
	CHECK(bf_png_read_signature(&s) == BF_OK);
	bf_png_chunk_t c1, c2, c3;
	CHECK(bf_png_read_chunk(&s, &c1) == BF_OK);
	CHECK(bf_png_read_chunk(&s, &c2) == BF_OK);
	size_t before_iend = s.byte_offset;
	CHECK(bf_png_read_chunk(&s, &c3) == BF_EOF);
	CHECK(s.byte_offset == before_iend);

	/* Invalid arguments */
	CHECK(bf_png_read_chunk(NULL, &c1) == BF_EINVAL);
	CHECK(bf_png_read_chunk(&s, NULL) == BF_EINVAL);
	CHECK(bf_png_verify_chunk_crc(NULL) == BF_EINVAL);
	CHECK(!bf_png_chunk_is(NULL, "IHDR"));

	printf("test_png_chunks_and_crc32 finished.\n");
}

int main(void)
{
	test_png_signature_and_errors();
	test_png_ihdr_and_validation();
	test_png_chunks_and_crc32();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All PNG parser tests passed.\n");
	return EXIT_SUCCESS;
}
