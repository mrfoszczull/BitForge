
#include <assert.h>
#include <bitforge/bitops.h>
#include <stdint.h>

int main(void) {
    uint8_t result = 0;

    assert(bf_rotate_left(0x96, 1, &result) == BF_OK);
    assert(result == 0x2D);

    assert(bf_rotate_left(0x96, 1, NULL) == BF_EINVAL);

    assert(bf_rotate_left(0x96, 0, &result) == BF_OK);
    assert(result == 0x96);

    assert(bf_rotate_left(0x96, 8, &result) == BF_OK);
    assert(result == 0x96);

    assert(bf_rotate_left(0x96, 9, &result) == BF_OK);
    assert(result == 0x2D);

    return 0;
}
