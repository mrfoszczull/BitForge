#include <bitforge/io.h>

#include <stdio.h>
#include <limits.h>

bf_status bf_io_open(bf_file_t *file, const char *path, bf_file_mode mode)
{
	const char *open_mode = NULL;
	FILE *handle;

	if (file == NULL || path == NULL || path[0] == '\0') {
		return BF_EINVAL;
	}

	if (file->handle != NULL) {
		return BF_EINVAL;
	}

	switch (mode) {
	case BF_FILE_READ:
		open_mode = "rb";
		break;

	case BF_FILE_WRITE:
		open_mode = "wb";
		break;

	case BF_FILE_APPEND:
		open_mode = "ab";
		break;

	case BF_FILE_READ_WRITE:
		open_mode = "r+b";
		break;

	default:
		return BF_EINVAL;
	}

	handle = fopen(path, open_mode);

	if (handle == NULL) {
		return BF_EIO;
	}

	file->handle = handle;

	return BF_OK;
}

bf_status bf_io_close(bf_file_t *file)
{
	FILE *handle;
	int result;

	if (file == NULL || file->handle == NULL) {
		return BF_EINVAL;
	}

	handle = (FILE *)file->handle;
	file->handle = NULL;

	result = fclose(handle);

	if (result != 0) {
		return BF_EIO;
	}

	return BF_OK;
}

bool bf_io_is_open(const bf_file_t *file)
{
	return file != NULL && file->handle != NULL;
}

bf_status bf_io_read(
    bf_file_t *file, uint8_t *data, size_t size, size_t *out_read)
{
	FILE *handle;
	size_t bytes_read;

	if (file == NULL || file->handle == NULL || out_read == NULL ||
	    (data == NULL && size > 0)) {
		return BF_EINVAL;
	}

	*out_read = 0;

	if (size == 0) {
		return BF_OK;
	}

	handle = (FILE *)file->handle;

	bytes_read = fread(data, 1, size, handle);

	*out_read = bytes_read;

	if (bytes_read == size) {
		return BF_OK;
	}

	if (ferror(handle)) {
		return BF_EIO;
	}

	if (feof(handle)) {
		return bytes_read > 0 ? BF_OK : BF_EOF;
	}

	return BF_EIO;
}

bf_status bf_io_write(
    bf_file_t *file, const uint8_t *data, size_t size, size_t *out_written)
{
	FILE *handle;
	size_t bytes_written;

	if (file == NULL || file->handle == NULL || out_written == NULL ||
	    (data == NULL && size > 0)) {
		return BF_EINVAL;
	}

	*out_written = 0;

	if (size == 0) {
		return BF_OK;
	}

	handle = (FILE *)file->handle;

	bytes_written = fwrite(data, 1, size, handle);

	*out_written = bytes_written;

	if (bytes_written != size) {
		return BF_EIO;
	}

	return BF_OK;
}

bf_status bf_io_seek(bf_file_t *file, size_t offset)
{
	FILE *handle;

	if (file == NULL || file->handle == NULL) {
		return BF_EINVAL;
	}

	if (offset > LONG_MAX) {
		return BF_EINVAL;
	}

	handle = (FILE *)file->handle;

	if (fseek(handle, (long)offset, SEEK_SET) != 0) {
		return BF_EIO;
	}

	return BF_OK;
}
