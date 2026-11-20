#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

static void test_parser_unsigned_integers(void)
{
	uint8_t buf[] = {
	    0xAB,	/* u8: 0xAB */
	    0x34, 0x12, /* u16_le: 0x1234, u16_be: 0x3412 */
	    0x12, 0x34, 0x56, 0x34,
	    0x12, /* u24_le: 0x123456, u24_be: 0x563412 */
	    0x56, 0x34, 0x12, 0x78, 0x56, 0x34,
	    0x12, /* u32_le: 0x12345678, u32_be: 0x78563412 */
	    0x78, 0x56, 0x34, 0x12, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02,
	    0x01, /* u64_le: 0x0102030405060708 */
	    0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
	    0x88 /* u64_be: 0x1122334455667788 */
	};
	bf_stream_t s;
	CHECK(bf_stream_init(&s, buf, sizeof(buf)) == BF_OK);

	uint8_t u8 = 0;
	uint16_t u16 = 0;
	uint32_t u24 = 0;
	uint32_t u32 = 0;
	uint64_t u64 = 0;

	CHECK(bf_parser_read_u8(&s, &u8) == BF_OK && u8 == 0xABU);
	CHECK(bf_parser_read_u16_le(&s, &u16) == BF_OK && u16 == 0x1234U);
	CHECK(bf_parser_read_u16_be(&s, &u16) == BF_OK && u16 == 0x1234U);
	CHECK(bf_parser_read_u24_le(&s, &u24) == BF_OK && u24 == 0x123456U);
	CHECK(bf_parser_read_u24_be(&s, &u24) == BF_OK && u24 == 0x563412U);
	CHECK(bf_parser_read_u32_le(&s, &u32) == BF_OK && u32 == 0x12345678U);
	CHECK(bf_parser_read_u32_be(&s, &u32) == BF_OK && u32 == 0x78563412U);
	CHECK(bf_parser_read_u64_le(&s, &u64) == BF_OK &&
	      u64 == 0x0102030405060708ULL);
	CHECK(bf_parser_read_u64_be(&s, &u64) == BF_OK &&
	      u64 == 0x1122334455667788ULL);

	CHECK(s.byte_offset == sizeof(buf));
	CHECK(bf_parser_read_u8(&s, &u8) == BF_EOF);

	printf("test_parser_unsigned_integers finished.\n");
}

static void test_parser_signed_integers(void)
{
	uint8_t buf[] = {
	    0xFF,					    /* i8: -1 */
	    0xFF, 0xFF,					    /* i16_le: -1 */
	    0xFF, 0xFF,					    /* i16_be: -1 */
	    0xFF, 0xFF, 0xFF,				    /* i24_le: -1 */
	    0xFF, 0xFF, 0xFF,				    /* i24_be: -1 */
	    0xFF, 0xFF, 0xFF, 0xFF,			    /* i32_le: -1 */
	    0xFF, 0xFF, 0xFF, 0xFF,			    /* i32_be: -1 */
	    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, /* i64_le: -1 */
	    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF  /* i64_be: -1 */
	};
	bf_stream_t s;
	CHECK(bf_stream_init(&s, buf, sizeof(buf)) == BF_OK);

	int8_t i8 = 0;
	int16_t i16 = 0;
	int32_t i24 = 0;
	int32_t i32 = 0;
	int64_t i64 = 0;

	CHECK(bf_parser_read_i8(&s, &i8) == BF_OK && i8 == -1);
	CHECK(bf_parser_read_i16_le(&s, &i16) == BF_OK && i16 == -1);
	CHECK(bf_parser_read_i16_be(&s, &i16) == BF_OK && i16 == -1);
	CHECK(bf_parser_read_i24_le(&s, &i24) == BF_OK && i24 == -1);
	CHECK(bf_parser_read_i24_be(&s, &i24) == BF_OK && i24 == -1);
	CHECK(bf_parser_read_i32_le(&s, &i32) == BF_OK && i32 == -1);
	CHECK(bf_parser_read_i32_be(&s, &i32) == BF_OK && i32 == -1);
	CHECK(bf_parser_read_i64_le(&s, &i64) == BF_OK && i64 == -1);
	CHECK(bf_parser_read_i64_be(&s, &i64) == BF_OK && i64 == -1);

	uint8_t mix[] = {
	    0x00, 0x80,	      /* i16_le: -32768 */
	    0x80, 0x00,	      /* i16_be: -32768 */
	    0x00, 0x00, 0x80, /* i24_le: -8388608 (0x800000) */
	    0x7F, 0xFF, 0xFF  /* i24_be: 8388607  (0x7FFFFF) */
	};
	CHECK(bf_stream_init(&s, mix, sizeof(mix)) == BF_OK);
	CHECK(bf_parser_read_i16_le(&s, &i16) == BF_OK && i16 == INT16_MIN);
	CHECK(bf_parser_read_i16_be(&s, &i16) == BF_OK && i16 == INT16_MIN);
	CHECK(bf_parser_read_i24_le(&s, &i24) == BF_OK && i24 == -8388608);
	CHECK(bf_parser_read_i24_be(&s, &i24) == BF_OK && i24 == 8388607);

	printf("test_parser_signed_integers finished.\n");
}

static void test_parser_bitfields_and_reserved(void)
{
	uint8_t buf[] = {0xB0U, 0x01U};
	bf_stream_t s;
	CHECK(bf_stream_init(&s, buf, sizeof(buf)) == BF_OK);

	bool flag = false;
	uint32_t bits = 0U;

	CHECK(bf_parser_read_flag(&s, &flag) == BF_OK && flag == true);
	CHECK(bf_parser_read_bits(&s, 3U, &bits) == BF_OK && bits == 0x3U);
	CHECK(bf_parser_check_reserved_bits(&s, 4U) == BF_OK);
	CHECK(s.byte_offset == 1U && s.bit_offset == 0U);

	CHECK(bf_parser_check_reserved_bits(&s, 8U) == BF_EFORMAT);
	CHECK(s.byte_offset == 1U && s.bit_offset == 0U);

	CHECK(bf_parser_check_reserved_bits(&s, 9U) == BF_EOF);

	CHECK(bf_parser_read_bits(&s, 0U, &bits) == BF_EINVAL);
	CHECK(bf_parser_read_bits(&s, 33U, &bits) == BF_EINVAL);
	CHECK(bf_parser_read_flag(&s, NULL) == BF_EINVAL);

	printf("test_parser_bitfields_and_reserved finished.\n");
}

static void test_parser_validation_and_errors(void)
{
	uint8_t buf[] = {'B', 'F', 0x00, 0x00, 0x01, 0x02, 0x03, 0x04};
	bf_stream_t s;
	CHECK(bf_stream_init(&s, buf, sizeof(buf)) == BF_OK);

	const uint8_t magic_ok[] = {'B', 'F'};
	const uint8_t magic_bad[] = {'X', 'Y'};

	CHECK(bf_parser_require_bytes(&s, 8U) == BF_OK);
	CHECK(bf_parser_require_bytes(&s, 9U) == BF_EOF);

	CHECK(bf_parser_expect_bytes(&s, magic_bad, 2U) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	CHECK(bf_parser_expect_bytes(&s, magic_ok, 2U) == BF_OK);
	CHECK(s.byte_offset == 2U);

	CHECK(bf_parser_expect_u32_be(&s, 0xDEADBEEFU) == BF_EFORMAT);
	CHECK(s.byte_offset == 2U);

	CHECK(bf_parser_expect_u32_be(&s, 0x00000102U) == BF_OK);
	CHECK(s.byte_offset == 6U);

	CHECK(bf_parser_expect_u32_le(&s, 0x04030201U) == BF_EOF);
	CHECK(s.byte_offset == 6U);

	CHECK(bf_stream_seek(&s, 0U, 1U) == BF_OK);
	uint8_t u8 = 0U;
	CHECK(bf_parser_read_u8(&s, &u8) == BF_EINVAL);
	CHECK(bf_parser_expect_bytes(&s, magic_ok, 2U) == BF_EINVAL);
	CHECK(bf_parser_expect_u32_be(&s, 0U) == BF_EINVAL);
	CHECK(bf_parser_require_bytes(&s, 1U) == BF_EINVAL);
	CHECK(bf_parser_read_u8(NULL, &u8) == BF_EINVAL);
	CHECK(bf_parser_read_u8(&s, NULL) == BF_EINVAL);

	printf("test_parser_validation_and_errors finished.\n");
}

int main(void)
{
	test_parser_unsigned_integers();
	test_parser_signed_integers();
	test_parser_bitfields_and_reserved();
	test_parser_validation_and_errors();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All parser tests passed.\n");
	return EXIT_SUCCESS;
}
