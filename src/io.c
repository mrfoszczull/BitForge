#include <bitforge/io.h>
#include <bitforge/status.h>

#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
	FILE *stream;
	bf_file_mode mode;
	bool last_operation_was_write;
} bf_file_internal_t;

bf_status bf_io_open(bf_file_t *file, const char *path, bf_file_mode mode)
{
	bf_file_internal_t *internal;
	const char *open_mode;
	FILE *stream;

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

	stream = fopen(path, open_mode);
	if (stream == NULL) {
		return BF_EIO;
	}

	internal = malloc(sizeof(*internal));
	if (internal == NULL) {
		(void)fclose(stream);
		return BF_ENOMEM;
	}

	internal->stream = stream;
	internal->mode = mode;
	internal->last_operation_was_write = false;

	file->handle = internal;

	return BF_OK;
}

bf_status bf_io_close(bf_file_t *file)
{
	bf_file_internal_t *internal;
	int result;

	if (file == NULL || file->handle == NULL) {
		return BF_EINVAL;
	}

	internal = (bf_file_internal_t *)file->handle;
	file->handle = NULL;

	result = fclose(internal->stream);
	free(internal);

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
	bf_file_internal_t *internal;
	size_t bytes_read;

	if (file == NULL || file->handle == NULL || out_read == NULL ||
	    (data == NULL && size > 0)) {
		return BF_EINVAL;
	}

	*out_read = 0;

	internal = (bf_file_internal_t *)file->handle;

	if (internal->mode != BF_FILE_READ &&
	    internal->mode != BF_FILE_READ_WRITE) {
		return BF_EINVAL;
	}

	if (size == 0) {
		return BF_OK;
	}

	if (internal->mode == BF_FILE_READ_WRITE &&
	    internal->last_operation_was_write) {
		if (fseek(internal->stream, 0, SEEK_CUR) != 0) {
			return BF_EIO;
		}
	}

	bytes_read = fread(data, 1, size, internal->stream);

	internal->last_operation_was_write = false;
	*out_read = bytes_read;

	if (bytes_read == size) {
		return BF_OK;
	}

	if (ferror(internal->stream)) {
		return BF_EIO;
	}

	if (feof(internal->stream)) {
		return bytes_read > 0 ? BF_OK : BF_EOF;
	}

	return BF_EIO;
}

bf_status bf_io_write(
    bf_file_t *file, const uint8_t *data, size_t size, size_t *out_written)
{
	bf_file_internal_t *internal;
	size_t bytes_written;

	if (file == NULL || file->handle == NULL || out_written == NULL ||
	    (data == NULL && size > 0)) {
		return BF_EINVAL;
	}

	*out_written = 0;

	internal = (bf_file_internal_t *)file->handle;

	if (internal->mode == BF_FILE_READ) {
		return BF_EINVAL;
	}

	if (size == 0) {
		return BF_OK;
	}

	if (internal->mode == BF_FILE_READ_WRITE &&
	    !internal->last_operation_was_write) {
		if (fseek(internal->stream, 0, SEEK_CUR) != 0) {
			return BF_EIO;
		}
	}

	bytes_written = fwrite(data, 1, size, internal->stream);

	internal->last_operation_was_write = true;
	*out_written = bytes_written;

	if (bytes_written != size) {
		return BF_EIO;
	}

	return BF_OK;
}

bf_status bf_io_seek(bf_file_t *file, size_t offset)
{
	bf_file_internal_t *internal;

	if (file == NULL || file->handle == NULL) {
		return BF_EINVAL;
	}

	if ((uintmax_t)offset > (uintmax_t)LONG_MAX) {
		return BF_EOVERFLOW;
	}

	internal = (bf_file_internal_t *)file->handle;

	if (fseek(internal->stream, (long)offset, SEEK_SET) != 0) {
		return BF_EIO;
	}

	internal->last_operation_was_write = false;

	return BF_OK;
}

bf_status bf_io_tell(const bf_file_t *file, size_t *out_position)
{
	bf_file_internal_t *internal;
	long position;

	if (file == NULL || file->handle == NULL || out_position == NULL) {
		return BF_EINVAL;
	}

	*out_position = 0;

	internal = (bf_file_internal_t *)file->handle;

	position = ftell(internal->stream);
	if (position < 0) {
		return BF_EIO;
	}

	if ((uintmax_t)position > (uintmax_t)SIZE_MAX) {
		return BF_EOVERFLOW;
	}

	*out_position = (size_t)position;

	return BF_OK;
}

bf_status bf_io_size(bf_file_t *file, size_t *out_size)
{
	bf_file_internal_t *internal;
	long current_position;
	long file_size;

	if (file == NULL || file->handle == NULL || out_size == NULL) {
		return BF_EINVAL;
	}

	*out_size = 0;
	internal = (bf_file_internal_t *)file->handle;

	current_position = ftell(internal->stream);
	if (current_position < 0) {
		return BF_EIO;
	}

	if (fseek(internal->stream, 0, SEEK_END) != 0) {
		(void)fseek(internal->stream, current_position, SEEK_SET);
		return BF_EIO;
	}

	file_size = ftell(internal->stream);
	if (file_size < 0) {
		(void)fseek(internal->stream, current_position, SEEK_SET);
		return BF_EIO;
	}

	if (fseek(internal->stream, current_position, SEEK_SET) != 0) {
		return BF_EIO;
	}

	if ((uintmax_t)file_size > (uintmax_t)SIZE_MAX) {
		return BF_EOVERFLOW;
	}

	*out_size = (size_t)file_size;

	return BF_OK;
}
