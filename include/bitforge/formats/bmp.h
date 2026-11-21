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
 * @brief File header, BITMAPINFOHEADER DIB header, and pixel slice parser
 * @{
 */

#define BF_BMP_FILE_HEADER_LEN 14U
#define BF_BMP_INFO_HEADER_MIN_LEN 40U

typedef enum {
	BF_BMP_COMP_RGB = 0U,
	BF_BMP_COMP_RLE8 = 1U,
	BF_BMP_COMP_RLE4 = 2U,
	BF_BMP_COMP_BITFIELDS = 3U
} bf_bmp_compression_t;

/**
 * @brief Parsed BMP file header (14 bytes).
 */
typedef struct {
	uint32_t file_size; /**< Declared total file size in bytes. */
	uint16_t reserved1; /**< Reserved field 1. */
	uint16_t reserved2; /**< Reserved field 2. */
	uint32_t
	    pixel_offset; /**< Byte offset from start of file to pixel array. */
} bf_bmp_file_header_t;

/**
 * @brief Parsed DIB BITMAPINFOHEADER fields (40-byte core).
 */
typedef struct {
	uint32_t header_size; /**< DIB header size (>= 40). */
	int32_t width;	      /**< Image width in pixels (> 0). */
	int32_t height;	    /**< Image height (positive = bottom-up, negative =
			       top-down). */
	uint16_t planes;    /**< Color planes (must be 1). */
	uint16_t bit_count; /**< Bits per pixel (1, 4, 8, 16, 24, 32). */
	uint32_t compression; /**< Compression method (bf_bmp_compression_t). */
	uint32_t
	    image_size; /**< Pixel data size in bytes (may be 0 for BI_RGB). */
	int32_t x_pixels_per_meter;
	int32_t y_pixels_per_meter;
	uint32_t colors_used;
	uint32_t colors_important;
} bf_bmp_info_header_t;

/**
 * @brief Zero-copy view of pixel data inside the underlying stream buffer.
 */
typedef struct {
	const uint8_t
	    *data;     /**< Pointer to pixel bytes inside stream buffer. */
	size_t size;   /**< Available pixel byte count from pixel_offset. */
	bool top_down; /**< True when height < 0. */
	uint32_t abs_height; /**< Positive row count. */
} bf_bmp_pixels_t;

/**
 * @brief Verifies the 2-byte BMP signature ("BM") without advancing on
 * mismatch.
 */
bf_status bf_bmp_read_signature(bf_stream_t *stream);

/**
 * @brief Reads and validates the 14-byte BMP file header.
 */
bf_status bf_bmp_read_file_header(
    bf_stream_t *stream, bf_bmp_file_header_t *out_header);

/**
 * @brief Reads and validates a BITMAPINFOHEADER (>= 40 bytes).
 *
 * Advances stream to byte_offset + header_size on success.
 */
bf_status bf_bmp_read_info_header(
    bf_stream_t *stream, bf_bmp_info_header_t *out_info);

/**
 * @brief Computes row stride in bytes for uncompressed packed rows.
 *
 * Row size is padded to a 4-byte boundary: ((width * bit_count + 31) / 32) * 4.
 */
bf_status bf_bmp_row_stride(
    uint32_t width, uint16_t bit_count, size_t *out_stride);

/**
 * @brief Seeks to pixel_offset and returns a zero-copy pixel slice view.
 */
bf_status bf_bmp_get_pixels(bf_stream_t *stream,
    const bf_bmp_file_header_t *file_hdr, const bf_bmp_info_header_t *info_hdr,
    bf_bmp_pixels_t *out_pixels);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_FORMATS_BMP_H */
