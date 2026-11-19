#include <bitforge/pattern.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

static void test_pattern_init_valid(void)
{
	bf_pattern_t pattern;
	uint8_t data[] = {0xAB, 0xCD};

	CHECK(bf_pattern_init(&pattern, data, 8) == BF_OK);
	CHECK(pattern.bit_count == 8);
	CHECK(pattern.mask == 0xFFULL);
	CHECK(pattern.data == 0xABULL);

	/* 12 bits: 0xABC -> 1010 1011 1100 */
	CHECK(bf_pattern_init(&pattern, data, 12) == BF_OK);
	CHECK(pattern.bit_count == 12);
	CHECK(pattern.mask == 0xFFFULL);
	CHECK(pattern.data == 0xABCULL);

	/* Full 64-bit pattern */
	uint8_t data64[8] = {0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF};
	CHECK(bf_pattern_init(&pattern, data64, 64) == BF_OK);
	CHECK(pattern.bit_count == 64);
	CHECK(pattern.mask == 0xFFFFFFFFFFFFFFFFULL);
	CHECK(pattern.data == 0x0123456789ABCDEFULL);

	printf("test_pattern_init_valid finished.\n");
}

static void test_pattern_init_invalid_args(void)
{
	bf_pattern_t pattern;
	uint8_t data[] = {0xFF};

	CHECK(bf_pattern_init(NULL, data, 8) == BF_EINVAL);
	CHECK(bf_pattern_init(&pattern, NULL, 8) == BF_EINVAL);
	CHECK(bf_pattern_init(&pattern, data, 0) == BF_EINVAL);
	CHECK(bf_pattern_init(&pattern, data, 65) == BF_EINVAL);

	printf("test_pattern_init_invalid_args finished.\n");
}

static void test_pattern_init_masked_valid(void)
{
	bf_pattern_t pattern;
	uint8_t data[] = {0xAA};
	uint8_t mask[] = {0xF0};

	CHECK(bf_pattern_init_masked(&pattern, data, mask, 8) == BF_OK);
	CHECK(pattern.bit_count == 8);
	CHECK(pattern.mask == 0xF0ULL);
	CHECK(pattern.data == 0xA0ULL);

	printf("test_pattern_init_masked_valid finished.\n");
}

static void test_pattern_init_masked_invalid_args(void)
{
	bf_pattern_t pattern;
	uint8_t data[] = {0xFF};
	uint8_t mask[] = {0xFF};

	CHECK(bf_pattern_init_masked(NULL, data, mask, 8) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, NULL, mask, 8) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, data, NULL, 8) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, data, mask, 0) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, data, mask, 65) == BF_EINVAL);

	printf("test_pattern_init_masked_invalid_args finished.\n");
}

static void test_pattern_parse_hex(void)
{
	bf_pattern_t pattern;

	/* Wildcards at start, middle, and end */
	CHECK(bf_pattern_parse_hex(&pattern, "?? 8B ?? 89 45 ??") == BF_OK);
	CHECK(pattern.bit_count == 48);
	CHECK(pattern.mask == 0x00FF00FFFF00ULL);
	CHECK(pattern.data == 0x008B00894500ULL);

	/* Max 8 bytes (64 bits) with extra whitespace */
	CHECK(bf_pattern_parse_hex(&pattern, "  01 02 03 04 05 06 07 08  ") ==
	      BF_OK);
	CHECK(pattern.bit_count == 64);
	CHECK(pattern.mask == 0xFFFFFFFFFFFFFFFFULL);
	CHECK(pattern.data == 0x0102030405060708ULL);

	/* Invalid syntax and arguments */
	CHECK(bf_pattern_parse_hex(NULL, "48 8B") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, NULL) == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "   ") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "4") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "488B") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "ZZ") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "?0") == BF_EINVAL);
	CHECK(bf_pattern_parse_hex(&pattern, "01 02 03 04 05 06 07 08 09") ==
	      BF_EINVAL);

	printf("test_pattern_parse_hex finished.\n");
}

static void test_pattern_match(void)
{
	uint8_t buffer[] = {0x48, 0x8B, 0x10};
	bf_stream_t stream;
	CHECK(bf_stream_init(&stream, buffer, sizeof(buffer)) == BF_OK);

	bf_pattern_t pat;
	CHECK(bf_pattern_parse_hex(&pat, "48 8B ??") == BF_OK);

	bool matched = false;
	CHECK(bf_pattern_match(&pat, &stream, &matched) == BF_OK);
	CHECK(matched == true);
	/* Stream position must not change */
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	/* Advance 1 bit -> no longer matches at current position */
	CHECK(bf_stream_skip_bits(&stream, 1) == BF_OK);
	CHECK(bf_pattern_match(&pat, &stream, &matched) == BF_OK);
	CHECK(matched == false);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 1);

	/* Invalid arguments */
	CHECK(bf_pattern_match(NULL, &stream, &matched) == BF_EINVAL);
	CHECK(bf_pattern_match(&pat, NULL, &matched) == BF_EINVAL);
	CHECK(bf_pattern_match(&pat, &stream, NULL) == BF_EINVAL);

	printf("test_pattern_match finished.\n");
}

static void test_stream_find_pattern_valid(void)
{
	uint8_t buffer[] = {0xDE, 0xAD, 0xBE, 0xEF};
	bf_stream_t stream;
	bf_stream_init(&stream, buffer, sizeof(buffer));

	bf_pattern_t pattern;
	uint8_t pat_data[] = {0xAD};
	CHECK(bf_pattern_init(&pattern, pat_data, 8) == BF_OK);

	/* Should find 0xAD at byte 1, bit 0 */
	CHECK(bf_stream_find_pattern(&stream, &pattern) == BF_OK);
	CHECK(stream.byte_offset == 1);
	CHECK(stream.bit_offset == 0);

	printf("test_stream_find_pattern_valid finished.\n");
}

static void test_stream_find_pattern_unaligned_and_64bit(void)
{
	/* 0x80 = 10000000, 0x40 = 01000000 -> 0x01 (00000001) starts at bit 2
	 */
	uint8_t buffer[] = {0x80, 0x40};
	bf_stream_t stream;
	CHECK(bf_stream_init(&stream, buffer, sizeof(buffer)) == BF_OK);

	bf_pattern_t pat8;
	uint8_t d8[] = {0x01};
	CHECK(bf_pattern_init(&pat8, d8, 8) == BF_OK);
	CHECK(bf_stream_find_pattern(&stream, &pat8) == BF_OK);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 2);

	printf("test_stream_find_pattern_unaligned_and_64bit finished.\n");
}

static void test_stream_find_pattern_masked(void)
{
	uint8_t buffer[] = {0x12, 0x34, 0x56};
	bf_stream_t stream;
	bf_stream_init(&stream, buffer, sizeof(buffer));

	bf_pattern_t pattern;
	uint8_t pat_data[] = {0x30};
	uint8_t pat_mask[] = {0xF0};
	CHECK(bf_pattern_init_masked(&pattern, pat_data, pat_mask, 8) == BF_OK);

	CHECK(bf_stream_find_pattern(&stream, &pattern) == BF_OK);
	CHECK(stream.byte_offset == 1);
	CHECK(stream.bit_offset == 0);

	printf("test_stream_find_pattern_masked finished.\n");
}

static void test_stream_find_pattern_from_and_all(void)
{
	uint8_t buffer[] = {0xAA, 0x00, 0xAA, 0x00, 0xAA};
	bf_stream_t stream;
	CHECK(bf_stream_init(&stream, buffer, sizeof(buffer)) == BF_OK);

	bf_pattern_t pat;
	CHECK(bf_pattern_parse_hex(&pat, "AA") == BF_OK);

	size_t out_byte = 99;
	uint8_t out_bit = 99;
	CHECK(bf_stream_find_pattern_from(
		  &stream, &pat, 1, 0, &out_byte, &out_bit) == BF_OK);
	CHECK(out_byte == 2);
	CHECK(out_bit == 0);
	/* Stream remains at start */
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	/* Find all: matches at bit offsets 0, 16, 32 */
	size_t positions[2] = {0};
	size_t total_count = 0;
	CHECK(bf_stream_find_pattern_all(
		  &stream, &pat, positions, 2, &total_count) == BF_OK);
	CHECK(total_count == 3);
	CHECK(positions[0] == 0);
	CHECK(positions[1] == 16);
	/* Stream remains at start */
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	/* Query total count with capacity == 0 and positions == NULL */
	total_count = 0;
	CHECK(bf_stream_find_pattern_all(
		  &stream, &pat, NULL, 0, &total_count) == BF_OK);
	CHECK(total_count == 3);

	printf("test_stream_find_pattern_from_and_all finished.\n");
}

static void test_stream_find_pattern_not_found(void)
{
	uint8_t buffer[] = {0x00, 0x00, 0x00};
	bf_stream_t stream;
	bf_stream_init(&stream, buffer, sizeof(buffer));

	bf_pattern_t pattern;
	uint8_t pat_data[] = {0xFF};
	CHECK(bf_pattern_init(&pattern, pat_data, 8) == BF_OK);

	CHECK(bf_stream_find_pattern(&stream, &pattern) == BF_ENOTFOUND);

	printf("test_stream_find_pattern_not_found finished.\n");
}

static void test_stream_find_pattern_invalid_args(void)
{
	uint8_t buffer[] = {0x12};
	bf_stream_t stream;
	bf_stream_init(&stream, buffer, sizeof(buffer));

	bf_pattern_t pattern;
	uint8_t pat_data[] = {0x12};
	bf_pattern_init(&pattern, pat_data, 8);

	size_t out_byte = 0;
	uint8_t out_bit = 0;
	size_t out_count = 0;

	CHECK(bf_stream_find_pattern(NULL, &pattern) == BF_EINVAL);
	CHECK(bf_stream_find_pattern(&stream, NULL) == BF_EINVAL);

	CHECK(bf_stream_find_pattern_from(
		  NULL, &pattern, 0, 0, &out_byte, &out_bit) == BF_EINVAL);
	CHECK(bf_stream_find_pattern_from(
		  &stream, NULL, 0, 0, &out_byte, &out_bit) == BF_EINVAL);
	CHECK(bf_stream_find_pattern_from(
		  &stream, &pattern, 0, 8, &out_byte, &out_bit) == BF_EINVAL);

	CHECK(bf_stream_find_pattern_all(NULL, &pattern, NULL, 0, &out_count) ==
	      BF_EINVAL);
	CHECK(bf_stream_find_pattern_all(&stream, NULL, NULL, 0, &out_count) ==
	      BF_EINVAL);
	CHECK(bf_stream_find_pattern_all(
		  &stream, &pattern, NULL, 1, &out_count) == BF_EINVAL);
	CHECK(bf_stream_find_pattern_all(&stream, &pattern, NULL, 0, NULL) ==
	      BF_EINVAL);

	printf("test_stream_find_pattern_invalid_args finished.\n");
}

int main(void)
{
	printf("Running pattern and stream search tests...\n");

	test_pattern_init_valid();
	test_pattern_init_invalid_args();
	test_pattern_init_masked_valid();
	test_pattern_init_masked_invalid_args();

	test_pattern_parse_hex();
	test_pattern_match();
	test_stream_find_pattern_valid();
	test_stream_find_pattern_unaligned_and_64bit();

	test_stream_find_pattern_masked();
	test_stream_find_pattern_from_and_all();
	test_stream_find_pattern_not_found();
	test_stream_find_pattern_invalid_args();

	if (failures > 0) {
		fprintf(stderr, "Tests failed with %d error(s).\n", failures);
	} else {
		printf("All pattern tests passed successfully!\n");
	}

	return failures;
}
