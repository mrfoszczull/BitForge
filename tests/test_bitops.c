#include <bitforge/bitops.h>
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

static void test_rotate_left(void)
{
	uint8_t result = 0;

	CHECK(bf_rotate_left(0x96, 1, &result) == BF_OK);
	CHECK(result == 0x2D);

	CHECK(bf_rotate_left(0x96, 1, NULL) == BF_EINVAL);

	CHECK(bf_rotate_left(0x96, 0, &result) == BF_OK);
	CHECK(result == 0x96);

	CHECK(bf_rotate_left(0x96, 8, &result) == BF_OK);
	CHECK(result == 0x96);

	CHECK(bf_rotate_left(0x96, 9, &result) == BF_OK);
	CHECK(result == 0x2D);

	CHECK(bf_rotate_left(0x80, 1, &result) == BF_OK);
	CHECK(result == 0x01);

	CHECK(bf_rotate_left(0x01, 7, &result) == BF_OK);
	CHECK(result == 0x80);

	CHECK(bf_rotate_left(0x96, 16, &result) == BF_OK);
	CHECK(result == 0x96);

	CHECK(bf_rotate_left(0x00, 3, &result) == BF_OK);
	CHECK(result == 0x00);

	CHECK(bf_rotate_left(0xFF, 3, &result) == BF_OK);
	CHECK(result == 0xFF);
}

static void test_bf_rotate_right(void)
{
	uint8_t result = 0;

	CHECK(bf_rotate_right(0x0D, 1, &result) == BF_OK);
	CHECK(result == 0x86);

	CHECK(bf_rotate_right(0x0D, 2, &result) == BF_OK);
	CHECK(result == 0x43);

	CHECK(bf_rotate_right(0x0D, 0, &result) == BF_OK);
	CHECK(result == 0x0D);

	CHECK(bf_rotate_right(0x0D, 8, &result) == BF_OK);
	CHECK(result == 0x0D);

	CHECK(bf_rotate_right(0x0D, 9, &result) == BF_OK);
	CHECK(result == 0x86);

	CHECK(bf_rotate_right(0x0D, 1, NULL) == BF_EINVAL);
}

static void test_get(void)
{
	uint8_t value = 0xA5;
	bool result = false;

	CHECK(bf_get(&value, 0, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_get(&value, 1, &result) == BF_OK);
	CHECK(result == false);

	CHECK(bf_get(&value, 2, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_get(&value, 7, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_get(NULL, 0, &result) == BF_EINVAL);
	CHECK(bf_get(&value, 8, &result) == BF_EINVAL);
	CHECK(bf_get(&value, 0, NULL) == BF_EINVAL);
}

static void test_set(void)
{
	uint8_t value = 0x00;

	CHECK(bf_set(&value, 3) == BF_OK);
	CHECK(value == 0x08);

	CHECK(bf_set(&value, 3) == BF_OK);
	CHECK(value == 0x08);

	CHECK(bf_set(&value, 0) == BF_OK);
	CHECK(value == 0x09);

	CHECK(bf_set(&value, 7) == BF_OK);
	CHECK(value == 0x89);

	CHECK(bf_set(NULL, 0) == BF_EINVAL);
	CHECK(bf_set(&value, 8) == BF_EINVAL);
}

static void test_clear(void)
{
	uint8_t value = 0x0F;

	CHECK(bf_clear(&value, 1) == BF_OK);
	CHECK(value == 13);

	CHECK(bf_clear(&value, 7) == BF_OK);
	CHECK(value == 13);

	value = 0x80;
	CHECK(bf_clear(&value, 7) == BF_OK);
	CHECK(value == 0x00);

	CHECK(bf_clear(NULL, 1) == BF_EINVAL);
	CHECK(bf_clear(&value, 8) == BF_EINVAL);
}

static void test_toggle(void)
{
	uint8_t value = 0x04;

	CHECK(bf_toggle(&value, 2) == BF_OK);
	CHECK(value == 0);

	CHECK(bf_toggle(&value, 2) == BF_OK);
	CHECK(value == 4);

	value = 0x00;
	CHECK(bf_toggle(&value, 7) == BF_OK);
	CHECK(value == 0x80);

	CHECK(bf_toggle(NULL, 2) == BF_EINVAL);
	CHECK(bf_toggle(&value, 8) == BF_EINVAL);
}

static void test_count_set_bits(void)
{
	uint8_t value = 0x0F;
	uint8_t result = 0;

	CHECK(bf_count_set_bits(&value, &result) == BF_OK);
	CHECK(result == 4);

	value = 0x00;
	CHECK(bf_count_set_bits(&value, &result) == BF_OK);
	CHECK(result == 0);

	value = 0xFF;
	CHECK(bf_count_set_bits(&value, &result) == BF_OK);
	CHECK(result == 8);

	CHECK(bf_count_set_bits(NULL, &result) == BF_EINVAL);
	CHECK(bf_count_set_bits(&value, NULL) == BF_EINVAL);
}

static void test_is_power_of_two(void)
{
	bool result = false;

	CHECK(bf_is_power_of_two(16, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_is_power_of_two(128, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_is_power_of_two(32, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_is_power_of_two(64, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_is_power_of_two(26, &result) == BF_OK);
	CHECK(result == false);

	CHECK(bf_is_power_of_two(3, &result) == BF_OK);
	CHECK(result == false);

	CHECK(bf_is_power_of_two(255, &result) == BF_OK);
	CHECK(result == false);

	CHECK(bf_is_power_of_two(127, &result) == BF_OK);
	CHECK(result == false);

	CHECK(bf_is_power_of_two(0, &result) == BF_OK);
	CHECK(result == false);

	CHECK(bf_is_power_of_two(1, &result) == BF_OK);
	CHECK(result == true);

	CHECK(bf_is_power_of_two(128, NULL) == BF_EINVAL);
}

static void test_reverse_bits(void)
{
	uint8_t result = 0;

	CHECK(bf_reverse_bits(0x01, &result) == BF_OK);
	CHECK(result == 0x80);

	CHECK(bf_reverse_bits(0x00, &result) == BF_OK);
	CHECK(result == 0x00);

	CHECK(bf_reverse_bits(0x69, &result) == BF_OK);
	CHECK(result == 0x96);

	CHECK(bf_reverse_bits(0xFF, &result) == BF_OK);
	CHECK(result == 0xFF);

	CHECK(bf_reverse_bits(0x00, NULL) == BF_EINVAL);
}

static void test_bf_get_bits(void)
{
	uint8_t result = 0;

	CHECK(bf_get_bits(0xD6, 1, 1, &result) == BF_OK);
	CHECK(result == 1);

	CHECK(bf_get_bits(0xD6, 2, 3, &result) == BF_OK);
	CHECK(result == 0x05);

	CHECK(bf_get_bits(0xD6, 0, 8, &result) == BF_OK);
	CHECK(result == 0xD6);

	CHECK(bf_get_bits(0xD6, 0, 4, &result) == BF_OK);
	CHECK(result == 0x06);

	CHECK(bf_get_bits(0xD6, 0, 0, &result) == BF_EINVAL);
	CHECK(bf_get_bits(0xD6, 0, 9, &result) == BF_EINVAL);

	CHECK(bf_get_bits(0xD6, 7, 2, &result) == BF_EINVAL);
	CHECK(bf_get_bits(0xD6, 8, 1, &result) == BF_EINVAL);

	CHECK(bf_get_bits(0xD6, 0, 4, NULL) == BF_EINVAL);
}

static void test_bf_set_bits(void)
{
	uint8_t value = 0x02;

	CHECK(bf_set_bits(&value, 0, 1) == BF_OK);
	CHECK(value == 0x03);

	value = 0x02;
	CHECK(bf_set_bits(&value, 2, 3) == BF_OK);
	CHECK(value == 0x1E);

	value = 0x00;
	CHECK(bf_set_bits(&value, 0, 8) == BF_OK);
	CHECK(value == 0xFF);

	value = 0xF0;
	CHECK(bf_set_bits(&value, 0, 4) == BF_OK);
	CHECK(value == 0xFF);

	CHECK(bf_set_bits(&value, 0, 0) == BF_EINVAL);
	CHECK(bf_set_bits(&value, 0, 9) == BF_EINVAL);

	CHECK(bf_set_bits(&value, 7, 2) == BF_EINVAL);
	CHECK(bf_set_bits(&value, 8, 1) == BF_EINVAL);

	CHECK(bf_set_bits(NULL, 0, 1) == BF_EINVAL);
}

static void test_bf_clear_bits(void)
{
	uint8_t value = 0xFF;

	CHECK(bf_clear_bits(&value, 0, 1) == BF_OK);
	CHECK(value == 0xFE);

	value = 0xFF;
	CHECK(bf_clear_bits(&value, 2, 3) == BF_OK);
	CHECK(value == 0xE3);

	value = 0xFF;
	CHECK(bf_clear_bits(&value, 0, 8) == BF_OK);
	CHECK(value == 0x00);

	value = 0xFF;
	CHECK(bf_clear_bits(&value, 4, 4) == BF_OK);
	CHECK(value == 0x0F);

	value = 0x00;
	CHECK(bf_clear_bits(&value, 0, 4) == BF_OK);
	CHECK(value == 0x00);

	CHECK(bf_clear_bits(&value, 0, 0) == BF_EINVAL);
	CHECK(bf_clear_bits(&value, 0, 9) == BF_EINVAL);

	CHECK(bf_clear_bits(&value, 7, 2) == BF_EINVAL);
	CHECK(bf_clear_bits(&value, 8, 1) == BF_EINVAL);

	CHECK(bf_clear_bits(NULL, 0, 1) == BF_EINVAL);
}

static void test_bf_toggle_bits(void)
{
	uint8_t value = 0x00;

	CHECK(bf_toggle_bits(&value, 0, 1) == BF_OK);
	CHECK(value == 0x01);

	value = 0x00;
	CHECK(bf_toggle_bits(&value, 2, 3) == BF_OK);
	CHECK(value == 0x1C);

	value = 0x00;
	CHECK(bf_toggle_bits(&value, 0, 8) == BF_OK);
	CHECK(value == 0xFF);

	CHECK(bf_toggle_bits(&value, 0, 8) == BF_OK);
	CHECK(value == 0x00);

	value = 0x0F;
	CHECK(bf_toggle_bits(&value, 4, 4) == BF_OK);
	CHECK(value == 0xFF);

	CHECK(bf_toggle_bits(&value, 0, 0) == BF_EINVAL);
	CHECK(bf_toggle_bits(&value, 0, 9) == BF_EINVAL);

	/* Zakres wykraczający poza 8 bitów. */
	CHECK(bf_toggle_bits(&value, 7, 2) == BF_EINVAL);
	CHECK(bf_toggle_bits(&value, 8, 1) == BF_EINVAL);

	CHECK(bf_toggle_bits(NULL, 0, 1) == BF_EINVAL);
}

static void test_bf_swap_bits(void)
{
	uint8_t value = 0x01;

	CHECK(bf_swap_bits(&value, 0, 1) == BF_OK);
	CHECK(value == 0x02);

	value = 0x81;
	CHECK(bf_swap_bits(&value, 0, 7) == BF_OK);
	CHECK(value == 0x81);

	value = 0x00;
	CHECK(bf_swap_bits(&value, 2, 5) == BF_OK);
	CHECK(value == 0x00);

	value = 0xFF;
	CHECK(bf_swap_bits(&value, 2, 5) == BF_OK);
	CHECK(value == 0xFF);

	value = 0xA5;
	CHECK(bf_swap_bits(&value, 3, 3) == BF_OK);
	CHECK(value == 0xA5);

	CHECK(bf_swap_bits(&value, 8, 0) == BF_EINVAL);
	CHECK(bf_swap_bits(&value, 0, 8) == BF_EINVAL);

	CHECK(bf_swap_bits(NULL, 0, 1) == BF_EINVAL);
}

int main(void)
{
	test_rotate_left();
	test_bf_rotate_right();
	test_get();
	test_set();
	test_clear();
	test_toggle();
	test_count_set_bits();
	test_is_power_of_two();
	test_reverse_bits();
	test_bf_get_bits();
	test_bf_set_bits();
	test_bf_clear_bits();
	test_bf_toggle_bits();
	test_bf_swap_bits();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All bitops tests passed.\n");
	return EXIT_SUCCESS;
}
