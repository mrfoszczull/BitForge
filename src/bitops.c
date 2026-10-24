#include <bitforge/bitops.h>
#include <bitforge/status.h>

bf_status bf_get(const uint8_t *value, size_t bit_index, bool *result) {
    if (value == NULL || bit_index >= 8 || result == NULL) {
        return BF_EINVAL;
    }

    *result = (*value >> bit_index) & 1;

    return BF_OK;
}
