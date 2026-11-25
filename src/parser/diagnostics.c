#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <bitforge/parser.h>

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

static bool diag_stream_valid(const bf_stream_t *stream)
{
	if (stream == NULL) {
		return false;
	}
	if (stream->data == NULL && stream->size > 0U) {
		return false;
	}
	if (stream->byte_offset > stream->size) {
		return false;
	}
	if (stream->bit_offset > 7U) {
		return false;
	}
	if (stream->byte_offset == stream->size && stream->bit_offset != 0U) {
		return false;
	}
	return true;
}

void bf_parser_diag_reset(bf_parser_diag_t *diag)
{
	if (diag == NULL) {
		return;
	}

	diag->byte_offset = 0U;
	diag->bit_offset = 0U;
	diag->status = BF_OK;
	diag->message = NULL;
}

bf_status bf_parser_diag_set(bf_parser_diag_t *diag, const bf_stream_t *stream,
    bf_status status, const char *message)
{
	if (diag == NULL || !diag_stream_valid(stream)) {
		return BF_EINVAL;
	}

	diag->byte_offset = stream->byte_offset;
	diag->bit_offset = stream->bit_offset;
	diag->status = status;
	diag->message = (message != NULL) ? message : bf_status_string(status);

	return BF_OK;
}

bf_status bf_parser_diag_format(
    const bf_parser_diag_t *diag, char *out_buf, size_t buf_size)
{
	if (diag == NULL || out_buf == NULL || buf_size == 0U) {
		return BF_EINVAL;
	}

	const char *status_name = bf_status_string(diag->status);
	const char *msg = (diag->message != NULL) ? diag->message : status_name;

	int written = snprintf(out_buf, buf_size, "[%zu:%u] %s (%d): %s",
	    diag->byte_offset, (unsigned int)diag->bit_offset, status_name,
	    (int)diag->status, msg);

	if (written < 0) {
		out_buf[0] = '\0';
		return BF_EIO;
	}

	if ((size_t)written >= buf_size) {
		return BF_EOVERFLOW;
	}

	return BF_OK;
}
