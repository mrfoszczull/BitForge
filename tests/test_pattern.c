#include <bitforge/pattern.h>
#include <bitforge/status.h>
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
	uint8_t data[] = {0xAB, 0xCD}; // 10101011 11001101

	// Test 8-bit pattern initialization
	CHECK(bf_pattern_init(&pattern, data, 8) == BF_OK);
	CHECK(pattern.bit_count == 8);
	CHECK(pattern.mask == 0xFFULL);
	CHECK(pattern.data == 0xABULL);

	// Test 12-bit pattern initialization (0xABC -> 1010 1011 1100)
	CHECK(bf_pattern_init(&pattern, data, 12) == BF_OK);
	CHECK(pattern.bit_count == 12);
	CHECK(pattern.mask == 0xFFFULL);
	CHECK(pattern.data == 0xABCULL);

	// Test full 64-bit pattern initialization
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

	// Null pointer validations
	CHECK(bf_pattern_init(NULL, data, 8) == BF_EINVAL);
	CHECK(bf_pattern_init(&pattern, NULL, 8) == BF_EINVAL);

	// Invalid bit counts (0 or exceeding BF_MAX_PATTERN_BITS)
	CHECK(bf_pattern_init(&pattern, data, 0) == BF_EINVAL);
	CHECK(bf_pattern_init(&pattern, data, 65) == BF_EINVAL);

	printf("test_pattern_init_invalid_args finished.\n");
}

static void test_pattern_init_masked_valid(void)
{
	bf_pattern_t pattern;
	uint8_t data[] = {0xAA}; // 10101010
	uint8_t mask[] = {
	    0xF0}; // 11110000 (Upper 4 bits checked, lower 4 wildcard)

	CHECK(bf_pattern_init_masked(&pattern, data, mask, 8) == BF_OK);
	CHECK(pattern.bit_count == 8);
	CHECK(pattern.mask == 0xF0ULL);
	// Data should be masked upon init: 0xAA & 0xF0 = 0xA0
	CHECK(pattern.data == 0xA0ULL);

	printf("test_pattern_init_masked_valid finished.\n");
}

static void test_pattern_init_masked_invalid_args(void)
{
	bf_pattern_t pattern;
	uint8_t data[] = {0xFF};
	uint8_t mask[] = {0xFF};

	// Null pointer validations
	CHECK(bf_pattern_init_masked(NULL, data, mask, 8) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, NULL, mask, 8) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, data, NULL, 8) == BF_EINVAL);

	// Invalid bit counts
	CHECK(bf_pattern_init_masked(&pattern, data, mask, 0) == BF_EINVAL);
	CHECK(bf_pattern_init_masked(&pattern, data, mask, 65) == BF_EINVAL);

	printf("test_pattern_init_masked_invalid_args finished.\n");
}

static void test_stream_find_pattern_valid(void)
{
	uint8_t buffer[] = {0xDE, 0xAD, 0xBE, 0xEF};
	bf_stream_t stream;
	bf_stream_init(&stream, buffer, sizeof(buffer));

	bf_pattern_t pattern;
	uint8_t pat_data[] = {0xAD};
	CHECK(bf_pattern_init(&pattern, pat_data, 8) == BF_OK);

	// Should find 0xAD at index 1
	CHECK(bf_stream_find_pattern(&stream, &pattern) == BF_OK);

	printf("test_stream_find_pattern_valid finished.\n");
}

static void test_stream_find_pattern_masked(void)
{
	uint8_t buffer[] = {0x12, 0x34, 0x56};
	bf_stream_t stream;
	bf_stream_init(&stream, buffer, sizeof(buffer));

	bf_pattern_t pattern;
	uint8_t pat_data[] = {0x30}; // Looking for upper nibble '3'
	uint8_t pat_mask[] = {0xF0}; // Mask checks only top 4 bits
	CHECK(bf_pattern_init_masked(&pattern, pat_data, pat_mask, 8) == BF_OK);

	// 0x34 masked with 0xF0 equals 0x30, so it matches
	CHECK(bf_stream_find_pattern(&stream, &pattern) == BF_OK);

	printf("test_stream_find_pattern_masked finished.\n");
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

	CHECK(bf_stream_find_pattern(NULL, &pattern) == BF_EINVAL);
	CHECK(bf_stream_find_pattern(&stream, NULL) == BF_EINVAL);

	printf("test_stream_find_pattern_invalid_args finished.\n");
}

int main(void)
{
	printf("Running pattern and stream search tests...\n");

	test_pattern_init_valid();
	test_pattern_init_invalid_args();
	test_pattern_init_masked_valid();
	test_pattern_init_masked_invalid_args();
	test_stream_find_pattern_valid();
	test_stream_find_pattern_masked();
	test_stream_find_pattern_not_found();
	test_stream_find_pattern_invalid_args();

	if (failures > 0) {
		fprintf(stderr, "Tests failed with %d error(s).\n", failures);
	} else {
		printf("All pattern tests passed successfully!\n");
	}

	return failures;
}
