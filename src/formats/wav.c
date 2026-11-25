#include <bitforge/formats/wav.h>
#include <bitforge/parser.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

static bool stream_aligned_ready(const bf_stream_t *stream)
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
	if (stream->bit_offset != 0U) {
		return false;
	}
	return true;
}

static bool fmt_params_valid(const bf_wav_fmt_t *fmt)
{
	if (fmt == NULL || fmt->num_channels == 0U || fmt->sample_rate == 0U ||
	    fmt->block_align == 0U || fmt->bits_per_sample == 0U) {
		return false;
	}

	switch (fmt->audio_format) {
	case BF_WAV_FORMAT_PCM:
	case BF_WAV_FORMAT_IEEE_FLOAT:
	case BF_WAV_FORMAT_EXTENSIBLE:
		break;
	default:
		return false;
	}

	/* Expected minimum frame size: ceil((channels * bits_per_sample) / 8)
	 */
	uint64_t total_bits =
	    (uint64_t)fmt->num_channels * (uint64_t)fmt->bits_per_sample;
	uint64_t min_frame_bytes = (total_bits + 7ULL) / 8ULL;
	if ((uint64_t)fmt->block_align < min_frame_bytes) {
		return false;
	}

	/* Verify byte_rate does not overflow when checked against sample_rate
	 */
	uint64_t expected_rate =
	    (uint64_t)fmt->sample_rate * (uint64_t)fmt->block_align;
	if (fmt->byte_rate == 0U || expected_rate > (uint64_t)UINT32_MAX ||
	    (uint64_t)fmt->byte_rate != expected_rate) {
		return false;
	}

	return true;
}

bool bf_wav_chunk_is(const bf_wav_chunk_t *chunk, const char four_cc[4])
{
	if (chunk == NULL || four_cc == NULL) {
		return false;
	}
	return memcmp(chunk->id, four_cc, 4U) == 0;
}

bf_status bf_wav_read_riff_header(
    bf_stream_t *stream, bf_wav_riff_header_t *out_header)
{
	static const uint8_t k_riff_tag[4] = {'R', 'I', 'F', 'F'};
	static const uint8_t k_wave_tag[4] = {'W', 'A', 'V', 'E'};

	if (!stream_aligned_ready(stream) || out_header == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t probe = *stream;
	bf_wav_riff_header_t hdr;
	bf_status status;

	status = bf_parser_expect_bytes(&probe, k_riff_tag, sizeof(k_riff_tag));
	if (status != BF_OK) {
		return status;
	}

	status = bf_parser_read_u32_le(&probe, &hdr.riff_size);
	if (status != BF_OK) {
		return status;
	}

	/* riff_size must at least include the 4-byte "WAVE" tag */
	if (hdr.riff_size < 4U) {
		return BF_EFORMAT;
	}

	status = bf_parser_expect_bytes(&probe, k_wave_tag, sizeof(k_wave_tag));
	if (status != BF_OK) {
		return status;
	}

	*out_header = hdr;
	*stream = probe;
	return BF_OK;
}

bf_status bf_wav_read_chunk(bf_stream_t *stream, bf_wav_chunk_t *out_chunk)
{
	if (!stream_aligned_ready(stream) || out_chunk == NULL) {
		return BF_EINVAL;
	}

	if (stream->byte_offset == stream->size) {
		return BF_EOF;
	}

	bf_stream_t probe = *stream;
	uint8_t id_buf[4] = {0U};
	uint32_t size = 0U;
	bf_status status;

	if (4U > probe.size - probe.byte_offset) {
		return BF_EOF;
	}
	memcpy(id_buf, probe.data + probe.byte_offset, 4U);
	probe.byte_offset += 4U;

	status = bf_parser_read_u32_le(&probe, &size);
	if (status != BF_OK) {
		return status;
	}

	if ((size_t)size > probe.size - probe.byte_offset) {
		return BF_EOF;
	}

	const uint8_t *payload =
	    (size > 0U) ? (probe.data + probe.byte_offset) : NULL;

	/* RIFF chunks are padded to an even byte boundary on disk */
	size_t padded = (size_t)size + ((size_t)size & 1U);
	if (padded < (size_t)size) {
		return BF_EOVERFLOW;
	}

	size_t remaining = probe.size - probe.byte_offset;
	if (padded <= remaining) {
		probe.byte_offset += padded;
	} else {
		/* Trailing pad byte may be omitted at the very end of a file */
		probe.byte_offset += (size_t)size;
	}

	out_chunk->size = size;
	memcpy(out_chunk->id, id_buf, 4U);
	out_chunk->id[4] = '\0';
	out_chunk->data = payload;

	*stream = probe;
	return BF_OK;
}

bf_status bf_wav_parse_fmt(const bf_wav_chunk_t *chunk, bf_wav_fmt_t *out_fmt)
{
	if (chunk == NULL || out_fmt == NULL) {
		return BF_EINVAL;
	}
	if (!bf_wav_chunk_is(chunk, "fmt ") ||
	    chunk->size < BF_WAV_FMT_MIN_LEN) {
		return BF_EFORMAT;
	}
	if (chunk->data == NULL) {
		return BF_EINVAL;
	}

	bf_stream_t s;
	bf_status status = bf_stream_init(&s, chunk->data, chunk->size);
	if (status != BF_OK) {
		return status;
	}

	bf_wav_fmt_t fmt;
	status = bf_parser_read_u16_le(&s, &fmt.audio_format);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&s, &fmt.num_channels);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&s, &fmt.sample_rate);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u32_le(&s, &fmt.byte_rate);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&s, &fmt.block_align);
	if (status != BF_OK) {
		return status;
	}
	status = bf_parser_read_u16_le(&s, &fmt.bits_per_sample);
	if (status != BF_OK) {
		return status;
	}

	if (!fmt_params_valid(&fmt)) {
		return BF_EFORMAT;
	}

	*out_fmt = fmt;
	return BF_OK;
}

bf_status bf_wav_get_data(const bf_wav_chunk_t *chunk, bf_wav_data_t *out_data)
{
	if (chunk == NULL || out_data == NULL) {
		return BF_EINVAL;
	}
	if (!bf_wav_chunk_is(chunk, "data")) {
		return BF_EFORMAT;
	}
	if (chunk->size > 0U && chunk->data == NULL) {
		return BF_EINVAL;
	}

	out_data->data = chunk->data;
	out_data->size = (size_t)chunk->size;
	return BF_OK;
}
