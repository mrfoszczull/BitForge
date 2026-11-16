
#ifndef BITFORGE_IO_H
#define BITFORGE_IO_H

#include <bitforge/status.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup io File I/O
 * @brief File input and output operations
 * @{
 */

/**
 * @brief Represents an opened file.
 *
 * The internal file handle is managed by the BitForge I/O module.
 */
typedef struct {
	FILE *handle;
} bf_file_t;

/**
 * @brief File opening modes.
 */
typedef enum {
	BF_FILE_READ,	/**< Open an existing file for reading. */
	BF_FILE_WRITE,	/**< Open for writing, truncating existing content. */
	BF_FILE_APPEND, /**< Open for appending data. */
	BF_FILE_READ_WRITE /**< Open for reading and writing. */
} bf_file_mode;

/**
 * @brief Opens a file.
 *
 * @param file Pointer to the file structure.
 * @param path Path to the file.
 * @param mode File opening mode.
 * @return BF_OK on success, BF_EINVAL on invalid arguments,
 *         or an appropriate error status.
 */
bf_status bf_io_open(bf_file_t *file, const char *path, bf_file_mode mode);

/**
 * @brief Closes an opened file.
 *
 * @param file Pointer to the file structure.
 * @return BF_OK on success, BF_EINVAL on invalid arguments,
 *         or an appropriate error status.
 */
bf_status bf_io_close(bf_file_t *file);

/**
 * @brief Reads data from a file.
 *
 * @param file Pointer to the opened file.
 * @param data Destination buffer.
 * @param size Maximum number of bytes to read.
 * @param out_read Number of bytes actually read.
 * @return BF_OK on success, BF_EOF when no more data is available,
 *         or an appropriate error status.
 */
bf_status bf_io_read(
    bf_file_t *file, uint8_t *data, size_t size, size_t *out_read);

/**
 * @brief Writes data to a file.
 *
 * @param file Pointer to the opened file.
 * @param data Source buffer.
 * @param size Number of bytes to write.
 * @param out_written Number of bytes actually written.
 * @return BF_OK on success or an appropriate error status.
 */
bf_status bf_io_write(
    bf_file_t *file, const uint8_t *data, size_t size, size_t *out_written);

/**
 * @brief Changes the current file position.
 *
 * @param file Pointer to the opened file.
 * @param offset Target offset in bytes.
 * @return BF_OK on success or an appropriate error status.
 */
bf_status bf_io_seek(bf_file_t *file, size_t offset);

/**
 * @brief Gets the current file position.
 *
 * @param file Pointer to the opened file.
 * @param out_position Pointer to receive the current position.
 * @return BF_OK on success or an appropriate error status.
 */
bf_status bf_io_tell(const bf_file_t *file, size_t *out_position);

/**
 * @brief Gets the file size.
 *
 * @param file Pointer to the opened file.
 * @param out_size Pointer to receive the file size.
 * @return BF_OK on success or an appropriate error status.
 */
bf_status bf_io_size(bf_file_t *file, size_t *out_size);

/**
 * @brief Checks whether a file is currently open.
 *
 * @param file Pointer to the file structure.
 * @return true if the file is open, false otherwise.
 */
bool bf_io_is_open(const bf_file_t *file);

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BITFORGE_IO_H */
