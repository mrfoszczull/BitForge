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

int main(void) {
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

    if (failures != 0) {
        fprintf(stderr, "%d check(s) failed\n", failures);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}
