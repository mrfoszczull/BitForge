#ifndef BITFORGE_PARSER_H
#define BITFORGE_PARSER_H

#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup parser Generic Parser
 * @brief Endian-aware integer, bitfield, and validation primitives over streams
 * @{
 */

/**
 * @brief Byte order (endianness) for multi-byte values.
 */
typedef enum {
	BF_ENDIAN_LITTLE, /**< Least significant byte first. */
	BF_ENDIAN_BIG	  /**< Most significant byte first. */
} bf_endian_t;

/* --- Unsigned integers (aligned byte reads) --- */

bf_status bf_parser_read_u8(bf_stream_t *stream, uint8_t *out_value);
bf_status bf_parser_read_u16_le(bf_stream_t *stream, uint16_t *out_value);
bf_status bf_parser_read_u16_be(bf_stream_t *stream, uint16_t *out_value);
bf_status bf_parser_read_u24_le(bf_stream_t *stream, uint32_t *out_value);
bf_status bf_parser_read_u24_be(bf_stream_t *stream, uint32_t *out_value);
bf_status bf_parser_read_u32_le(bf_stream_t *stream, uint32_t *out_value);
bf_status bf_parser_read_u32_be(bf_stream_t *stream, uint32_t *out_value);
bf_status bf_parser_read_u64_le(bf_stream_t *stream, uint64_t *out_value);
bf_status bf_parser_read_u64_be(bf_stream_t *stream, uint64_t *out_value);

/* --- Signed integers (aligned byte reads, two's complement) --- */

bf_status bf_parser_read_i8(bf_stream_t *stream, int8_t *out_value);
bf_status bf_parser_read_i16_le(bf_stream_t *stream, int16_t *out_value);
bf_status bf_parser_read_i16_be(bf_stream_t *stream, int16_t *out_value);
bf_status bf_parser_read_i24_le(bf_stream_t *stream, int32_t *out_value);
bf_status bf_parser_read_i24_be(bf_stream_t *stream, int32_t *out_value);
bf_status bf_parser_read_i32_le(bf_stream_t *stream, int32_t *out_value);
bf_status bf_parser_read_i32_be(bf_stream_t *stream, int32_t *out_value);
bf_status bf_parser_read_i64_le(bf_stream_t *stream, int64_t *out_value);
bf_status bf_parser_read_i64_be(bf_stream_t *stream, int64_t *out_value);

/* --- Bitfields --- */

/**
 * @brief Reads 1 to 32 bits from the stream in MSB-first order.
 */
bf_status bf_parser_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value);

/**
 * @brief Reads a single bit as a boolean flag.
 */
bf_status bf_parser_read_flag(bf_stream_t *stream, bool *out_flag);

/**
 * @brief Skips reserved bits and verifies they are all zero.
 *
 * Returns BF_EFORMAT if any reserved bit is non-zero.
 */
bf_status bf_parser_check_reserved_bits(bf_stream_t *stream, size_t bit_count);

/* --- Signature & value validation --- */

/**
 * @brief Reads an expected byte sequence and verifies equality.
 *
 * Returns BF_EFORMAT if read bytes do not match expected.
 */
bf_status bf_parser_expect_bytes(
    bf_stream_t *stream, const uint8_t *expected, size_t length);

/**
 * @brief Reads a big-endian uint32 and verifies it equals expected.
 *
 * Returns BF_EFORMAT on mismatch.
 */
bf_status bf_parser_expect_u32_be(bf_stream_t *stream, uint32_t expected);

/**
 * @brief Reads a little-endian uint32 and verifies it equals expected.
 *
 * Returns BF_EFORMAT on mismatch.
 */
bf_status bf_parser_expect_u32_le(bf_stream_t *stream, uint32_t expected);

/**
 * @brief Ensures at least required_bytes aligned bytes remain in the stream.
 */
bf_status bf_parser_require_bytes(
    const bf_stream_t *stream, size_t required_bytes);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_PARSER_H */
