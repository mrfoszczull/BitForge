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

int main(void)
{
	printf("Running pattern initialization tests...\n");

	test_pattern_init_valid();
	test_pattern_init_invalid_args();

	if (failures > 0) {
		fprintf(stderr, "Tests failed with %d error(s).\n", failures);
	} else {
		printf("All pattern tests passed successfully!\n");
	}

	return failures;
}
