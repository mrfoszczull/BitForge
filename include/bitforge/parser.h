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

/**
 * @brief Reads an unsigned 8-bit integer from a byte-aligned stream.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 1 byte remains.
 */
bf_status bf_parser_read_u8(bf_stream_t *stream, uint8_t *out_value);

/**
 * @brief Reads an unsigned 16-bit little-endian integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 2 bytes remain.
 */
bf_status bf_parser_read_u16_le(bf_stream_t *stream, uint16_t *out_value);

/**
 * @brief Reads an unsigned 16-bit big-endian integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 2 bytes remain.
 */
bf_status bf_parser_read_u16_be(bf_stream_t *stream, uint16_t *out_value);

/**
 * @brief Reads an unsigned 24-bit little-endian integer into `uint32_t`.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 3 bytes remain.
 */
bf_status bf_parser_read_u24_le(bf_stream_t *stream, uint32_t *out_value);

/**
 * @brief Reads an unsigned 24-bit big-endian integer into `uint32_t`.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 3 bytes remain.
 */
bf_status bf_parser_read_u24_be(bf_stream_t *stream, uint32_t *out_value);

/**
 * @brief Reads an unsigned 32-bit little-endian integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 4 bytes remain.
 */
bf_status bf_parser_read_u32_le(bf_stream_t *stream, uint32_t *out_value);

/**
 * @brief Reads an unsigned 32-bit big-endian integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 4 bytes remain.
 */
bf_status bf_parser_read_u32_be(bf_stream_t *stream, uint32_t *out_value);

/**
 * @brief Reads an unsigned 64-bit little-endian integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 8 bytes remain.
 */
bf_status bf_parser_read_u64_le(bf_stream_t *stream, uint64_t *out_value);

/**
 * @brief Reads an unsigned 64-bit big-endian integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 8 bytes remain.
 */
bf_status bf_parser_read_u64_be(bf_stream_t *stream, uint64_t *out_value);

/* --- Signed integers (aligned byte reads, two's complement) --- */

/**
 * @brief Reads a signed 8-bit two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 1 byte remains.
 */
bf_status bf_parser_read_i8(bf_stream_t *stream, int8_t *out_value);

/**
 * @brief Reads a signed 16-bit little-endian two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 2 bytes remain.
 */
bf_status bf_parser_read_i16_le(bf_stream_t *stream, int16_t *out_value);

/**
 * @brief Reads a signed 16-bit big-endian two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 2 bytes remain.
 */
bf_status bf_parser_read_i16_be(bf_stream_t *stream, int16_t *out_value);

/**
 * @brief Reads a signed 24-bit little-endian two's-complement integer into
 *        `int32_t`.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 3 bytes remain.
 */
bf_status bf_parser_read_i24_le(bf_stream_t *stream, int32_t *out_value);

/**
 * @brief Reads a signed 24-bit big-endian two's-complement integer into
 *        `int32_t`.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 3 bytes remain.
 */
bf_status bf_parser_read_i24_be(bf_stream_t *stream, int32_t *out_value);

/**
 * @brief Reads a signed 32-bit little-endian two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 4 bytes remain.
 */
bf_status bf_parser_read_i32_le(bf_stream_t *stream, int32_t *out_value);

/**
 * @brief Reads a signed 32-bit big-endian two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 4 bytes remain.
 */
bf_status bf_parser_read_i32_be(bf_stream_t *stream, int32_t *out_value);

/**
 * @brief Reads a signed 64-bit little-endian two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 8 bytes remain.
 */
bf_status bf_parser_read_i64_le(bf_stream_t *stream, int64_t *out_value);

/**
 * @brief Reads a signed 64-bit big-endian two's-complement integer.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 8 bytes remain.
 */
bf_status bf_parser_read_i64_be(bf_stream_t *stream, int64_t *out_value);

/* --- IEEE 754 floating-point numbers (aligned byte reads) --- */

/**
 * @brief Reads an IEEE 754 single-precision little-endian float.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 4 bytes remain.
 */
bf_status bf_parser_read_f32_le(bf_stream_t *stream, float *out_value);

/**
 * @brief Reads an IEEE 754 single-precision big-endian float.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 4 bytes remain.
 */
bf_status bf_parser_read_f32_be(bf_stream_t *stream, float *out_value);

/**
 * @brief Reads an IEEE 754 double-precision little-endian float.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 8 bytes remain.
 */
bf_status bf_parser_read_f64_le(bf_stream_t *stream, double *out_value);

/**
 * @brief Reads an IEEE 754 double-precision big-endian float.
 *
 * @param[in,out] stream    Byte-aligned input stream.
 * @param[out]    out_value Destination for the read value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if fewer than 8 bytes remain.
 */
bf_status bf_parser_read_f64_be(bf_stream_t *stream, double *out_value);

/* --- Strings (caller-owned output buffers, no allocation) --- */

/**
 * @brief Reads a fixed-length byte field into a NUL-terminated string buffer.
 *
 * Requires byte alignment (bit_offset == 0) and buf_size > length.
 * Stream advances by length bytes on success.
 *
 * @param[in,out] stream   Byte-aligned input stream.
 * @param[in]     length   Number of bytes to read from the stream.
 * @param[out]    out_buf  Destination string buffer.
 * @param[in]     buf_size Capacity of `out_buf` in bytes.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EOVERFLOW`.
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
 *
 * @param[in,out] stream   Byte-aligned input stream.
 * @param[out]    out_buf  Destination string buffer.
 * @param[in]     buf_size Capacity of `out_buf` in bytes.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EFORMAT`, or `BF_EOVERFLOW`.
 */
bf_status bf_parser_read_cstr(
    bf_stream_t *stream, char *out_buf, size_t buf_size);

/**
 * @brief Reads a length-prefixed ("Pascal") string: 1-byte length followed by
 *        that many bytes, stored as a NUL-terminated string in out_buf.
 *
 * Requires byte alignment (bit_offset == 0). Returns BF_EOVERFLOW if
 * prefixed length + 1 > buf_size.
 *
 * @param[in,out] stream   Byte-aligned input stream.
 * @param[out]    out_buf  Destination string buffer.
 * @param[in]     buf_size Capacity of `out_buf` in bytes.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EOVERFLOW`.
 */
bf_status bf_parser_read_pascal_u8_string(
    bf_stream_t *stream, char *out_buf, size_t buf_size);

/* --- Bitfields --- */

/**
 * @brief Reads an unsigned bitfield of `bit_count` bits from the stream.
 *
 * @param[in,out] stream    Input stream.
 * @param[in]     bit_count Number of bits to read (`1` to `32`).
 * @param[out]    out_value Destination for the read bitfield value.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments/width, or
 *         `BF_EOF` if fewer than `bit_count` bits remain.
 */
bf_status bf_parser_read_bits(
    bf_stream_t *stream, size_t bit_count, uint32_t *out_value);

/**
 * @brief Reads a single bit as a boolean flag.
 *
 * @param[in,out] stream   Input stream.
 * @param[out]    out_flag Destination set to `true` for bit `1` or `false`
 *                         for bit `0`.
 * @return `BF_OK` on success, `BF_EINVAL` on invalid arguments, or `BF_EOF`
 *         if no bits remain.
 */
bf_status bf_parser_read_flag(bf_stream_t *stream, bool *out_flag);

/**
 * @brief Reads and verifies that `bit_count` reserved bits are all zero.
 *
 * @param[in,out] stream    Input stream.
 * @param[in]     bit_count Number of reserved bits to inspect.
 * @return `BF_OK` if all bits are `0`, `BF_EFORMAT` if any bit is non-zero,
 *         `BF_EINVAL` on invalid arguments, or `BF_EOF` on truncated input.
 */
bf_status bf_parser_check_reserved_bits(bf_stream_t *stream, size_t bit_count);

/* --- Signature & value validation --- */

/**
 * @brief Verifies that the next `length` bytes in `stream` match `expected`.
 *
 * @param[in,out] stream   Byte-aligned input stream.
 * @param[in]     expected Expected byte sequence.
 * @param[in]     length   Number of bytes to compare.
 * @return `BF_OK` on match, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_parser_expect_bytes(
    bf_stream_t *stream, const uint8_t *expected, size_t length);

/**
 * @brief Verifies that the next 4 big-endian bytes equal `expected`.
 *
 * @param[in,out] stream   Byte-aligned input stream.
 * @param[in]     expected Expected 32-bit big-endian value.
 * @return `BF_OK` on match, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_parser_expect_u32_be(bf_stream_t *stream, uint32_t expected);

/**
 * @brief Verifies that the next 4 little-endian bytes equal `expected`.
 *
 * @param[in,out] stream   Byte-aligned input stream.
 * @param[in]     expected Expected 32-bit little-endian value.
 * @return `BF_OK` on match, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_parser_expect_u32_le(bf_stream_t *stream, uint32_t expected);

/**
 * @brief Ensures at least `required_bytes` remain unread in `stream`.
 *
 * @param[in] stream         Input stream to check.
 * @param[in] required_bytes Minimum number of remaining bytes required.
 * @return `BF_OK` if available, `BF_EINVAL`, `BF_EOVERFLOW`, or `BF_EOF`.
 */
bf_status bf_parser_require_bytes(
    const bf_stream_t *stream, size_t required_bytes);

/* --- Diagnostics --- */

/**
 * @brief Initializes or resets a diagnostic record.
 *
 * @param[out] diag Diagnostic structure to reset.
 */
void bf_parser_diag_reset(bf_parser_diag_t *diag);

/**
 * @brief Populates a diagnostic record from the current stream position.
 *
 * @param[out] diag    Diagnostic structure to populate.
 * @param[in]  stream  Optional stream providing current offsets.
 * @param[in]  status  Status code to record.
 * @param[in]  message Static diagnostic message string.
 * @return `BF_OK` on success or `BF_EINVAL` on invalid arguments.
 */
bf_status bf_parser_diag_set(bf_parser_diag_t *diag, const bf_stream_t *stream,
    bf_status status, const char *message);

/**
 * @brief Formats a human-readable summary into caller-provided buffer.
 *
 * @param[in]  diag     Diagnostic record to format.
 * @param[out] out_buf  Destination buffer.
 * @param[in]  buf_size Capacity of `out_buf` in bytes.
 * @return `BF_OK` on success or `BF_EINVAL` on invalid arguments.
 */
bf_status bf_parser_diag_format(
    const bf_parser_diag_t *diag, char *out_buf, size_t buf_size);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_PARSER_H */
