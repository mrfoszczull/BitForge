#ifndef BITFORGE_FORMATS_BMP_H
#define BITFORGE_FORMATS_BMP_H

#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup formats_bmp BMP Format Parser
 * @brief Zero-allocation C17 parser for BMP headers, DIB `BITMAPINFOHEADER`,
 *        row stride calculation, and zero-copy pixel slices.
 *
 * Parses the 2-byte `"BM"` signature, 14-byte BMP file header,
 * `BITMAPINFOHEADER` (`>= 40` bytes), packed 4-byte aligned row strides, and
 * zero-copy pixel slices over a `bf_stream_t`.
 * @{
 */

/** @name BMP Header Sizes
 * @{
 */
/** @brief Standard BMP file header size in bytes (`BITMAPFILEHEADER`). */
#define BF_BMP_FILE_HEADER_LEN 14U
/** @brief Minimum DIB `BITMAPINFOHEADER` size in bytes. */
#define BF_BMP_INFO_HEADER_MIN_LEN 40U
/** @} */

/**
 * @brief Supported BMP compression modes (`biCompression`).
 */
typedef enum {
	BF_BMP_COMP_RGB = 0U,  /**< Uncompressed RGB (`BI_RGB`). */
	BF_BMP_COMP_RLE8 = 1U, /**< 8-bit run-length encoding (`BI_RLE8`). */
	BF_BMP_COMP_RLE4 = 2U, /**< 4-bit run-length encoding (`BI_RLE4`). */
	BF_BMP_COMP_BITFIELDS = 3U /**< Bitfield masks (`BI_BITFIELDS`). */
} bf_bmp_compression_t;

/**
 * @brief Parsed BMP file header (`BITMAPFILEHEADER`, 14 bytes).
 */
typedef struct {
	uint32_t file_size; /**< Declared total file size in bytes. */
	uint16_t reserved1; /**< Reserved field 1. */
	uint16_t reserved2; /**< Reserved field 2. */
	uint32_t
	    pixel_offset; /**< Byte offset from start of file to pixel array. */
} bf_bmp_file_header_t;

/**
 * @brief Parsed DIB `BITMAPINFOHEADER` fields (40-byte core).
 */
typedef struct {
	uint32_t header_size; /**< DIB header size in bytes (`>= 40`). */
	int32_t width;	      /**< Image width in pixels (`> 0`). */
	int32_t height;	 /**< Image height (`> 0` bottom-up, `< 0` top-down). */
	uint16_t planes; /**< Color planes count (must be `1`). */
	uint16_t
	    bit_count; /**< Bits per pixel (`1`, `4`, `8`, `16`, `24`, `32`). */
	uint32_t
	    compression; /**< Compression method (`bf_bmp_compression_t`). */
	uint32_t image_size; /**< Pixel data size in bytes (may be `0` for
				`BI_RGB`). */
	int32_t x_pixels_per_meter; /**< Horizontal resolution in pixels per
				       meter. */
	int32_t
	    y_pixels_per_meter; /**< Vertical resolution in pixels per meter. */
	uint32_t colors_used;	/**< Number of color table entries used. */
	uint32_t colors_important; /**< Number of important colors. */
} bf_bmp_info_header_t;

/**
 * @brief Zero-copy view of pixel data inside the underlying stream buffer.
 *
 * @warning The `data` pointer remains valid only while the backing stream
 *          buffer stays alive and unmodified.
 */
typedef struct {
	const uint8_t
	    *data;     /**< Pointer to pixel bytes inside stream buffer. */
	size_t size;   /**< Available pixel byte count from `pixel_offset`. */
	bool top_down; /**< `true` when `height < 0`. */
	uint32_t abs_height; /**< Positive pixel row count. */
} bf_bmp_pixels_t;

/**
 * @brief Verifies the 2-byte BMP signature (`"BM"`) without advancing on
 *        mismatch.
 *
 * @param[in,out] stream Byte-aligned stream positioned at the BMP signature.
 * @return `BF_OK` on match, `BF_EINVAL` for invalid stream, `BF_EOF` on
 *         truncated input, or `BF_EFORMAT` on mismatch.
 */
bf_status bf_bmp_read_signature(bf_stream_t *stream);

/**
 * @brief Reads and validates the 14-byte BMP file header.
 *
 * @param[in,out] stream     Byte-aligned stream positioned at file header.
 * @param[out]    out_header Populated BMP file header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, or `BF_EFORMAT`.
 */
bf_status bf_bmp_read_file_header(
    bf_stream_t *stream, bf_bmp_file_header_t *out_header);

/**
 * @brief Reads and validates a `BITMAPINFOHEADER` (`>= 40` bytes).
 *
 * Advances `stream` to `byte_offset + header_size` on success.
 *
 * @param[in,out] stream   Byte-aligned stream positioned at DIB header.
 * @param[out]    out_info Populated DIB info header on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 *         `BF_EFORMAT`.
 */
bf_status bf_bmp_read_info_header(
    bf_stream_t *stream, bf_bmp_info_header_t *out_info);

/**
 * @brief Computes row stride in bytes for uncompressed packed rows.
 *
 * Row size is padded to a 4-byte boundary:
 * `((width * bit_count + 31) / 32) * 4`.
 *
 * @param[in]  width      Image width in pixels.
 * @param[in]  bit_count  Bits per pixel.
 * @param[out] out_stride Computed row stride in bytes on success.
 * @return `BF_OK` on success, `BF_EINVAL`, or `BF_EOVERFLOW`.
 */
bf_status bf_bmp_row_stride(
    uint32_t width, uint16_t bit_count, size_t *out_stride);

/**
 * @brief Seeks to `pixel_offset` and returns a zero-copy pixel slice view.
 *
 * @param[in,out] stream     Byte-aligned stream backing the BMP image.
 * @param[in]     file_hdr   Parsed BMP file header.
 * @param[in]     info_hdr   Parsed DIB info header.
 * @param[out]    out_pixels Populated zero-copy pixel view on success.
 * @return `BF_OK` on success, `BF_EINVAL`, `BF_EOF`, `BF_EOVERFLOW`, or
 *         `BF_EFORMAT`.
 */
bf_status bf_bmp_get_pixels(bf_stream_t *stream,
    const bf_bmp_file_header_t *file_hdr, const bf_bmp_info_header_t *info_hdr,
    bf_bmp_pixels_t *out_pixels);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_BMP_H */
