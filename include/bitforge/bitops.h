#ifndef BITOPS_H
#define BITOPS_H
#include "status.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

bf_status bf_get(const uint8_t *value, size_t bit_index, bool *result);
bf_status bf_set(uint8_t *value, size_t bit_index);
bf_status bf_clear(uint8_t *value, size_t bit_index);
bf_status bf_toggle(uint8_t *value, size_t bit_index);
bf_status bf_count_set_bits(const uint8_t *value, uint8_t *result);
bf_status bf_is_power_of_two(uint8_t value, bool *result);

#endif
