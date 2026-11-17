#include <bitforge/io.h>

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

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
		fclose(stream);
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

bf_status bf_io_tell(const bf_file_t *file, size_t *out_position)
{
	FILE *handle;
	long position;

	if (file == NULL || file->handle == NULL || out_position == NULL) {
		return BF_EINVAL;
	}

	*out_position = 0;

	handle = (FILE *)file->handle;

	position = ftell(handle);

	if (position < 0) {
		return BF_EIO;
	}

	*out_position = (size_t)position;

	return BF_OK;
}

bf_status bf_io_size(bf_file_t *file, size_t *out_size)
{
	FILE *handle;
	long current_position;
	long file_size;

	if (file == NULL || file->handle == NULL || out_size == NULL) {
		return BF_EINVAL;
	}

	*out_size = 0;

	handle = (FILE *)file->handle;

	current_position = ftell(handle);

	if (current_position < 0) {
		return BF_EIO;
	}

	if (fseek(handle, 0, SEEK_END) != 0) {
		return BF_EIO;
	}

	file_size = ftell(handle);

	if (file_size < 0) {
		(void)fseek(handle, current_position, SEEK_SET);
		return BF_EIO;
	}

	if (fseek(handle, current_position, SEEK_SET) != 0) {
		return BF_EIO;
	}

	*out_size = (size_t)file_size;

	return BF_OK;
}
