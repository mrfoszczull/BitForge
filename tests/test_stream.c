#include <bitforge/stream.h>
#include <bitforge/status.h>
#include <stdbool.h>
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

static void test_stream_init(void)
{
	uint8_t data[] = {0x12, 0x34};
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);
	CHECK(stream.data == data);
	CHECK(stream.size == 2);
	CHECK(stream.byte_offset == 0);
	CHECK(stream.bit_offset == 0);

	CHECK(bf_stream_init(NULL, data, sizeof(data)) == BF_EINVAL);
	CHECK(bf_stream_init(&stream, NULL, sizeof(data)) == BF_EINVAL);
}

static void test_stream_has_remaining(void)
{
	uint8_t data[] = {0xFF}; // 1 bajt = 8 bitów
	bf_stream_t stream;

	CHECK(bf_stream_init(&stream, data, sizeof(data)) == BF_OK);

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

	CHECK(bf_stream_read_bits(&stream, 0, &value) == BF_OK);
	CHECK(value == 0);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x1);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x2);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x3);

	CHECK(bf_stream_read_bits(&stream, 4, &value) == BF_OK);
	CHECK(value == 0x4);

	CHECK(bf_stream_read_bits(&stream, 1, &value) != BF_OK);

	CHECK(bf_stream_read_bits(NULL, 1, &value) == BF_EINVAL);
	CHECK(bf_stream_read_bits(&stream, 1, NULL) == BF_EINVAL);
}

int main(void)
{
	test_stream_init();
	test_stream_has_remaining();
	test_stream_read_bits();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All bfstream tests passed.\n");
	return EXIT_SUCCESS;
}
