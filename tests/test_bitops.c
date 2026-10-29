#include "bitforge/status.h"
#include <bitforge/bitops.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static int failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, __LINE__,   \
                    #cond);                                                    \
            failures++;                                                        \
        }                                                                      \
    } while (0)

static void test_rotate_left(void) {
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

static void test_get(void) {
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

static void test_set(void) {
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

static void test_clear(void) {
    uint8_t value = 0x0F;

    CHECK(bf_clear(&value, 1) == BF_OK);
    CHECK(value == 13);

    CHECK(bf_clear(&value, 7) == BF_OK);
    CHECK(value == 13);

    CHECK(bf_clear(NULL, 1) == BF_EINVAL);
    CHECK(bf_clear(&value, 8) == BF_EINVAL);
}

static void test_toggle(void) {
    uint8_t value = 0x04;

    CHECK(bf_toggle(&value, 2) == BF_OK);
    CHECK(value == 0);

    CHECK(bf_toggle(&value, 2) == BF_OK);
    CHECK(value == 4);

    CHECK(bf_toggle(NULL, 2) == BF_EINVAL);
    CHECK(bf_toggle(&value, 8) == BF_EINVAL);
}

int main(void) {
    test_rotate_left();
    test_get();
    test_set();
    test_clear();
    test_toggle();

    if (failures != 0) {
        fprintf(stderr, "%d check(s) failed\n", failures);
        return EXIT_FAILURE;
    }

    printf("All bitops tests passed.\n");
    return EXIT_SUCCESS;
}
