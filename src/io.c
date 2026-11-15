#include <bitforge/io.h>

#include <stdio.h>

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
