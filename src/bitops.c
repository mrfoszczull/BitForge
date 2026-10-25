#include <bitforge/bitops.h>
#include <bitforge/status.h>
#include <stddef.h>
#include <stdint.h>

bf_status bf_get(const uint8_t *value, size_t bit_index, bool *result) {
    if (value == NULL || bit_index >= 8 || result == NULL) {
        return BF_EINVAL;
    }

    *result = (*value >> bit_index) & 1;

    return BF_OK;
}

bf_status bf_set(uint8_t *value, size_t bit_index) {
    if (value == NULL || bit_index >= 8) {
        return BF_EINVAL;
    }

    *value |= (1u << bit_index);

    return BF_OK;
}

bf_status bf_clear(uint8_t *value, size_t bit_index) {
    if (value == NULL || bit_index >= 8) {
        return BF_EINVAL;
    }

    *value &= ~(1u << bit_index);

    return BF_OK;
}
