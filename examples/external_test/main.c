#include <bitforge/bitops.h>
#include <bitforge/status.h>

#include <stdint.h>
#include <stdio.h>

int main(void) {
    uint8_t result = 0;

    bf_status status = bf_reverse_bits(0x01, &result);
    if (status != BF_OK) {
        fprintf(stderr, "BitForge error: %s\n", bf_status_string(status));
        return 1;
    }

    printf("Reversed bits: 0x%02X\n", result);
    return 0;
}
