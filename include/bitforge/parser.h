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
 * @brief Endian-aware integer, bitfield, string, float, and validation over
 * streams
 * @{
 */

/**
 * @brief Byte order (endianness) for multi-byte values.
 */
typedef enum {
	BF_ENDIAN_LITTLE, /**< Least significant byte first. */
	BF_ENDIAN_BIG	  /**< Most significant byte first. */
} bf_endian_t;

/**
 * @brief Diagnostic context capturing stream offset and error details.
 */
typedef struct {
	size_t byte_offset; /**< Byte offset associated with the event/error. */
	uint8_t bit_offset; /**< Bit offset (0-7) associated with the
			       event/error. */
	bf_status status;   /**< Status code recorded. */
	const char
	    *message; /**< Static diagnostic message (never allocated). */
} bf_parser_diag_t;

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

/* --- IEEE 754 floating-point numbers (aligned byte reads) --- */

bf_status bf_parser_read_f32_le(bf_stream_t *stream, float *out_value);
bf_status bf_parser_read_f32_be(bf_stream_t *stream, float *out_value);
bf_status bf_parser_read_f64_le(bf_stream_t *stream, double *out_value);
bf_status bf_parser_read_f64_be(bf_stream_t *stream, double *out_value);

/* --- Strings (caller-owned output buffers, no allocation) --- */

/**
 * @brief Reads a fixed-length byte field into a NUL-terminated string buffer.
 *
 * Requires byte alignment (bit_offset == 0) and buf_size > length.
 * Stream advances by length bytes on success.
 */
bf_status bf_parser_read_fixed_string(
    bf_stream_t *stream, size_t length, char *out_buf, size_t buf_size);

/**
 * @brief Reads a NUL-terminated C string from the stream.
 *
 * Requires byte alignment (bit_offset == 0). Consumes bytes up to and
 * including the terminating '\0'. Returns BF_EFORMAT if no '\0' is found
 * before EOF, or BF_EOVERFLOW if the string (including '\0') does not fit in
 * out_buf.
 */
bf_status bf_parser_read_cstr(
    bf_stream_t *stream, char *out_buf, size_t buf_size);

/**
 * @brief Reads a length-prefixed ("Pascal") string: 1-byte length followed by
 *        that many bytes, stored as a NUL-terminated string in out_buf.
 *
 * Requires byte alignment (bit_offset == 0). Returns BF_EOVERFLOW if
 * prefixed length + 1 > buf_size.
 */
bf_status bf_parser_read_pascal_u8_string(
    bf_stream_t *stream, char *out_buf, size_t buf_size);

/* --- Bitfields --- */

bf_status bf_parser_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value);
bf_status bf_parser_read_flag(bf_stream_t *stream, bool *out_flag);
bf_status bf_parser_check_reserved_bits(bf_stream_t *stream, size_t bit_count);

/* --- Signature & value validation --- */

bf_status bf_parser_expect_bytes(
    bf_stream_t *stream, const uint8_t *expected, size_t length);
bf_status bf_parser_expect_u32_be(bf_stream_t *stream, uint32_t expected);
bf_status bf_parser_expect_u32_le(bf_stream_t *stream, uint32_t expected);
bf_status bf_parser_require_bytes(
    const bf_stream_t *stream, size_t required_bytes);

/* --- Diagnostics --- */

/**
 * @brief Initializes or resets a diagnostic record.
 */
void bf_parser_diag_reset(bf_parser_diag_t *diag);

/**
 * @brief Populates a diagnostic record from the current stream position.
 */
bf_status bf_parser_diag_set(bf_parser_diag_t *diag, const bf_stream_t *stream,
    bf_status status, const char *message);

/**
 * @brief Formats a human-readable summary into caller-provided buffer.
 */
bf_status bf_parser_diag_format(
    const bf_parser_diag_t *diag, char *out_buf, size_t buf_size);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_PARSER_H */
