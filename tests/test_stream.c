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

static void test_stream_init(void)
{
	uint8_t data[] = {0x12, 0x34};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);
	CHECK(stream.data == data);
	CHECK(stream.size == 2);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_init(&stream, NULL, 0) == BF_OK);
	CHECK(stream.data == NULL);
	CHECK(stream.size == 0);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_init(NULL, data, sizeof(data)) == BF_EINVAL);
	CHECK(bf_stream_init(&stream, NULL, sizeof(data)) == BF_EINVAL);
}

static void test_stream_has_remaining(void)
{
	uint8_t data[] = {0xFF};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	CHECK(bf_stream_has_remaining(&stream, 0) == true);
	CHECK(bf_stream_has_remaining(&stream, 8) == true);
	CHECK(bf_stream_has_remaining(&stream, 9) == false);

	uint32_t val = 0;
	CHECK(bf_stream_read_bits(&stream, 3, &val) == BF_OK);

	CHECK(bf_stream_has_remaining(&stream, 5) == true);
	CHECK(bf_stream_has_remaining(&stream, 6) == false);

	CHECK(bf_stream_has_remaining(NULL, 1) == false);
}

static void test_stream_read_bits(void)
{
	uint8_t data[] = {0x12, 0x34};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	uint32_t value = 0;

	CHECK(bf_stream_read_bits(&stream, 0, &value) == BF_EINVAL);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x1);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x2);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x3);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x4);

	CHECK(bf_stream_read_bits(&stream, 1, &value) == BF_EOF);
	CHECK(stream.byte_offset == 2);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_read_bits(&stream, 33, &value) == BF_EINVAL);
	CHECK(bf_stream_read_bits(NULL, 1, &value) == BF_EINVAL);
	CHECK(bf_stream_read_bits(&stream, 1, NULL) == BF_EINVAL);
}

static void test_stream_unaligned_reads(void)
{
	/* 0xAA = 1010 1010, 0x55 = 0101 0101 -> 1010101001010101 */
	uint8_t data[] = {0xAA, 0x55};
	bf_stream_t stream;
	uint32_t value = 0;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	/* 3 bits: 101 = 5 */
	CHECK(bf_stream_read_bits(&stream, 3, &value) == BF_OK);
	CHECK(value == 0x5);

	/* 7 bits: 0101001 = 0x29 */
	CHECK(bf_stream_read_bits(&stream, 7, &value) == BF_OK);
	CHECK(value == 0x29);

	/* remaining 6 bits: 010101 = 0x15 */
	CHECK(bf_stream_read_bits(&stream, 6, &value) == BF_OK);
	CHECK(value == 0x15);

	CHECK(bf_stream_read_bits(&stream, 1, &value) == BF_EOF);
}

static void test_stream_skip_bits(void)
{
	uint8_t data[] = {0xF0, 0x0F};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	CHECK(bf_stream_skip_bits(&stream, 0) == BF_OK);
	CHECK(bf_stream_skip_bits(&stream, 4) == BF_OK);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 4);

	uint32_t value = 0;
	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x0);

	CHECK(bf_stream_skip_bits(&stream, 100) == BF_EOF);
	CHECK(stream.byte_offset == 1);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_skip_bits(NULL, 1) == BF_EINVAL);

	bf_stream_t broken = {0};
	broken.size = 4;
	CHECK(bf_stream_skip_bits(&broken, 1) == BF_EINVAL);
}

static void test_stream_align(void)
{
	uint8_t data[] = {0xAA, 0x55};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	CHECK(bf_stream_align(&stream) == BF_OK);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_skip_bits(&stream, 3) == BF_OK);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 3);

	CHECK(bf_stream_align(&stream) == BF_OK);
	CHECK(stream.byte_offset == 1);
	CHECK(stream.bit_offset == 0);

	uint8_t byte_val = 0;
	CHECK(bf_stream_read_byte(&stream, &byte_val) == BF_OK);
	CHECK(byte_val == 0x55);

	CHECK(bf_stream_align(NULL) == BF_EINVAL);
}

static void test_stream_read_byte(void)
{
	uint8_t data[] = {0xAB};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	uint8_t val = 0;
	CHECK(bf_stream_read_byte(&stream, &val) == BF_OK);
	CHECK(val == 0xAB);
	CHECK(bf_stream_read_byte(&stream, &val) == BF_EOF);

	CHECK(bf_stream_read_byte(NULL, &val) == BF_EINVAL);
	CHECK(bf_stream_read_byte(&stream, NULL) == BF_EINVAL);
}

static void test_stream_read_bytes(void)
{
	uint8_t data[] = {0x10, 0x20, 0x30, 0x40};
	uint8_t out[4] = {0};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	CHECK(bf_stream_read_bytes(&stream, NULL, 0) == BF_OK);
	CHECK(bf_stream_read_bytes(&stream, out, 2) == BF_OK);
	CHECK(out[0] == 0x10 && out[1] == 0x20);
	CHECK(stream.byte_offset == 2);

	/* Unaligned read must fail with BF_EINVAL */
	CHECK(bf_stream_skip_bits(&stream, 1) == BF_OK);
	CHECK(bf_stream_read_bytes(&stream, out, 1) == BF_EINVAL);
	CHECK(stream.byte_offset == 2);
	CHECK(stream.bit_offset == 1);

	/* Align and read past end -> BF_EOF without moving position */
	CHECK(bf_stream_align(&stream) == BF_OK);
	CHECK(stream.byte_offset == 3);
	CHECK(bf_stream_read_bytes(&stream, out, 2) == BF_EOF);
	CHECK(stream.byte_offset == 3);

	/* Read remaining 1 byte */
	CHECK(bf_stream_read_bytes(&stream, out, 1) == BF_OK);
	CHECK(out[0] == 0x40);
	CHECK(stream.byte_offset == 4);

	CHECK(bf_stream_read_bytes(NULL, out, 1) == BF_EINVAL);
	CHECK(bf_stream_read_bytes(&stream, NULL, 1) == BF_EINVAL);
}

static void test_stream_read_bool(void)
{
	uint8_t data[] = {0x80};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	bool b_val = false;

	CHECK(bf_stream_read_bool(&stream, &b_val) == BF_OK);
	CHECK(b_val == true);

	for (int i = 0; i < 7; i++) {
		CHECK(bf_stream_read_bool(&stream, &b_val) == BF_OK);
		CHECK(b_val == false);
	}

	CHECK(bf_stream_read_bool(&stream, &b_val) == BF_EOF);

	CHECK(bf_stream_read_bool(NULL, &b_val) == BF_EINVAL);
	CHECK(bf_stream_read_bool(&stream, NULL) == BF_EINVAL);
}

static void test_stream_seek_and_tell(void)
{
	uint8_t data[] = {0xCA, 0xFE};
	bf_stream_t stream;
	size_t byte_off = 99;
	uint8_t bit_off = 99;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

	CHECK(bf_stream_seek(&stream, 1, 2) == BF_OK);
	CHECK(bf_stream_tell(&stream, &byte_off, &bit_off) == BF_OK);
	CHECK(byte_off == 1);
	CHECK(bit_off == 2);

	/* Seek to exact end (size, 0) is valid */
	CHECK(bf_stream_seek(&stream, sizeof(data), 0) == BF_OK);
	CHECK(bf_stream_has_remaining(&stream, 1) == false);
	CHECK(bf_stream_has_remaining(&stream, 0) == true);

	/* Invalid seeks leave position unchanged */
	CHECK(bf_stream_seek(&stream, sizeof(data), 1) == BF_EINVAL);
	CHECK(bf_stream_seek(&stream, sizeof(data) + 1, 0) == BF_EINVAL);
	CHECK(bf_stream_seek(&stream, 0, 8) == BF_EINVAL);
	CHECK(bf_stream_tell(&stream, &byte_off, &bit_off) == BF_OK);
	CHECK(byte_off == sizeof(data));
	CHECK(bit_off == 0);

	CHECK(bf_stream_seek(NULL, 0, 0) == BF_EINVAL);
	CHECK(bf_stream_tell(NULL, &byte_off, &bit_off) == BF_EINVAL);
	CHECK(bf_stream_tell(&stream, NULL, NULL) == BF_EINVAL);
}

static void test_stream_size_overflow(void)
{
	uint8_t data[] = {0x12, 0x34};
	bf_stream_t stream;
	uint32_t value = 0;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);
	CHECK(bf_stream_read_bits(&stream, 8, &value) == BF_OK);
	CHECK(value == 0x12);

	CHECK(bf_stream_has_remaining(&stream, SIZE_MAX) == false);
	CHECK(bf_stream_has_remaining(&stream, SIZE_MAX - 7) == false);
	CHECK(bf_stream_has_remaining(&stream, 8) == true);

	CHECK(bf_stream_skip_bits(&stream, SIZE_MAX) == BF_EOF);
	CHECK(stream.byte_offset == 1);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_read_bits(&stream, 8, &value) == BF_OK);
	CHECK(value == 0x34);
}

int main(void)
{
	test_stream_init();
	test_stream_has_remaining();
	test_stream_read_bits();
	test_stream_unaligned_reads();

	test_stream_skip_bits();
	test_stream_align();
	test_stream_read_byte();
	test_stream_read_bytes();

	test_stream_read_bool();
	test_stream_seek_and_tell();
	test_stream_size_overflow();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All bfstream tests passed.\n");
	return EXIT_SUCCESS;
}
