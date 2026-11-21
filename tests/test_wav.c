#include <bitforge/formats/wav.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

static void write_u16_le(uint8_t *dst, uint16_t val)
{
	dst[0] = (uint8_t)(val & 0xFFU);
	dst[1] = (uint8_t)((val >> 8) & 0xFFU);
}

static void write_u32_le(uint8_t *dst, uint32_t val)
{
	dst[0] = (uint8_t)(val & 0xFFU);
	dst[1] = (uint8_t)((val >> 8) & 0xFFU);
	dst[2] = (uint8_t)((val >> 16) & 0xFFU);
	dst[3] = (uint8_t)((val >> 24) & 0xFFU);
}

static size_t append_riff_chunk(uint8_t *buf, size_t buf_cap, size_t offset,
    const char id[4], const uint8_t *payload, uint32_t payload_len)
{
	size_t pad = (size_t)payload_len & 1U;
	size_t total = BF_WAV_CHUNK_HEADER_LEN + (size_t)payload_len + pad;
	if (offset + total > buf_cap) {
		return offset;
	}

	memcpy(buf + offset, id, 4U);
	write_u32_le(buf + offset + 4U, payload_len);
	if (payload_len > 0U && payload != NULL) {
		memcpy(buf + offset + BF_WAV_CHUNK_HEADER_LEN, payload,
		    payload_len);
	}
	if (pad > 0U) {
		buf[offset + BF_WAV_CHUNK_HEADER_LEN + (size_t)payload_len] =
		    0x00U;
	}

	return offset + total;
}

static size_t build_valid_pcm_wav(uint8_t *buf, size_t buf_cap)
{
	/* Standard 16-bit stereo 44100 Hz PCM fmt chunk (16B) */
	uint8_t fmt_payload[BF_WAV_FMT_MIN_LEN] = {0};
	write_u16_le(fmt_payload + 0U, BF_WAV_FORMAT_PCM);
	write_u16_le(fmt_payload + 2U, 2U);	 /* num_channels = 2 */
	write_u32_le(fmt_payload + 4U, 44100U);	 /* sample_rate = 44100 */
	write_u32_le(fmt_payload + 8U, 176400U); /* byte_rate = 44100 * 4 */
	write_u16_le(fmt_payload + 12U, 4U);	 /* block_align = 2 * 16 / 8 */
	write_u16_le(fmt_payload + 14U, 16U);	 /* bits_per_sample = 16 */

	/* Odd-length metadata chunk ("PAD ") to verify even-byte RIFF padding
	 */
	static const uint8_t odd_meta[3] = {'A', 'B', 'C'};

	/* 2 stereo 16-bit frames = 8 bytes of sample data */
	static const uint8_t pcm_samples[8] = {
	    0x01U, 0x00U, 0xFEU, 0xFFU, 0x10U, 0x00U, 0xF0U, 0xFFU};

	if (buf_cap < BF_WAV_RIFF_HEADER_LEN) {
		return 0U;
	}

	size_t pos = BF_WAV_RIFF_HEADER_LEN;
	pos = append_riff_chunk(
	    buf, buf_cap, pos, "fmt ", fmt_payload, sizeof(fmt_payload));
	pos = append_riff_chunk(
	    buf, buf_cap, pos, "PAD ", odd_meta, sizeof(odd_meta));
	pos = append_riff_chunk(
	    buf, buf_cap, pos, "data", pcm_samples, sizeof(pcm_samples));

	/* RIFF size covers everything after the 8-byte "RIFF" + size header */
	memcpy(buf, "RIFF", 4U);
	write_u32_le(buf + 4U, (uint32_t)(pos - 8U));
	memcpy(buf + 8U, "WAVE", 4U);

	return pos;
}

static void test_wav_riff_header_and_errors(void)
{
	uint8_t wav[256] = {0};
	size_t len = build_valid_pcm_wav(wav, sizeof(wav));
	CHECK(len > BF_WAV_RIFF_HEADER_LEN);

	bf_stream_t s;
	bf_wav_riff_header_t rhdr;
	CHECK(bf_stream_init(&s, wav, len) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_OK);
	CHECK(rhdr.riff_size == (uint32_t)(len - 8U));
	CHECK(s.byte_offset == BF_WAV_RIFF_HEADER_LEN);

	/* Bad RIFF tag -> BF_EFORMAT, offset unchanged */
	uint8_t bad_riff[16] = {
	    'F', 'F', 'I', 'R', 8U, 0U, 0U, 0U, 'W', 'A', 'V', 'E'};
	CHECK(bf_stream_init(&s, bad_riff, sizeof(bad_riff)) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	/* Bad WAVE form type -> BF_EFORMAT, offset unchanged */
	uint8_t bad_wave[16] = {
	    'R', 'I', 'F', 'F', 8U, 0U, 0U, 0U, 'A', 'V', 'I', ' '};
	CHECK(bf_stream_init(&s, bad_wave, sizeof(bad_wave)) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	/* riff_size < 4 -> BF_EFORMAT */
	uint8_t tiny_riff[12] = {
	    'R', 'I', 'F', 'F', 2U, 0U, 0U, 0U, 'W', 'A', 'V', 'E'};
	CHECK(bf_stream_init(&s, tiny_riff, sizeof(tiny_riff)) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_EFORMAT);
	CHECK(s.byte_offset == 0U);

	/* Truncated header -> BF_EOF */
	CHECK(bf_stream_init(&s, wav, 8U) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_EOF);
	CHECK(s.byte_offset == 0U);

	/* Invalid / NULL arguments -> BF_EINVAL */
	CHECK(bf_wav_read_riff_header(NULL, &rhdr) == BF_EINVAL);
	CHECK(bf_wav_read_riff_header(&s, NULL) == BF_EINVAL);

	printf("test_wav_riff_header_and_errors finished.\n");
}

static void test_wav_chunks_fmt_and_data(void)
{
	uint8_t wav[256] = {0};
	size_t len = build_valid_pcm_wav(wav, sizeof(wav));
	CHECK(len > 0U);

	bf_stream_t s;
	bf_wav_riff_header_t rhdr;
	CHECK(bf_stream_init(&s, wav, len) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_OK);

	bool saw_fmt = false;
	bool saw_pad = false;
	bool saw_data = false;
	bf_wav_fmt_t fmt;
	bf_wav_data_t data;

	for (;;) {
		bf_wav_chunk_t chunk;
		bf_status st = bf_wav_read_chunk(&s, &chunk);
		if (st == BF_EOF) {
			break;
		}
		CHECK(st == BF_OK);

		if (bf_wav_chunk_is(&chunk, "fmt ")) {
			saw_fmt = true;
			CHECK(bf_wav_parse_fmt(&chunk, &fmt) == BF_OK);
			CHECK(fmt.audio_format == BF_WAV_FORMAT_PCM);
			CHECK(fmt.num_channels == 2U);
			CHECK(fmt.sample_rate == 44100U);
			CHECK(fmt.byte_rate == 176400U);
			CHECK(fmt.block_align == 4U);
			CHECK(fmt.bits_per_sample == 16U);
		} else if (bf_wav_chunk_is(&chunk, "PAD ")) {
			saw_pad = true;
			CHECK(chunk.size == 3U);
			CHECK(memcmp(chunk.data, "ABC", 3U) == 0);
		} else if (bf_wav_chunk_is(&chunk, "data")) {
			saw_data = true;
			CHECK(bf_wav_get_data(&chunk, &data) == BF_OK);
			CHECK(data.size == 8U);
			CHECK(data.data != NULL);
			CHECK(data.data[0] == 0x01U);
		}
	}

	CHECK(saw_fmt && saw_pad && saw_data);

	/* Invalid fmt parameters (num_channels = 0) -> BF_EFORMAT */
	bf_wav_chunk_t bad_fmt_chunk;
	uint8_t bad_fmt_payload[BF_WAV_FMT_MIN_LEN] = {0};
	write_u16_le(bad_fmt_payload + 0U, BF_WAV_FORMAT_PCM);
	write_u16_le(bad_fmt_payload + 2U, 0U); /* invalid channels */
	write_u32_le(bad_fmt_payload + 4U, 44100U);
	write_u32_le(bad_fmt_payload + 8U, 88200U);
	write_u16_le(bad_fmt_payload + 12U, 2U);
	write_u16_le(bad_fmt_payload + 14U, 16U);

	memcpy(bad_fmt_chunk.id, "fmt ", 4U);
	bad_fmt_chunk.id[4] = '\0';
	bad_fmt_chunk.size = sizeof(bad_fmt_payload);
	bad_fmt_chunk.data = bad_fmt_payload;
	CHECK(bf_wav_parse_fmt(&bad_fmt_chunk, &fmt) == BF_EFORMAT);

	/* Wrong chunk ID passed to bf_wav_parse_fmt / bf_wav_get_data ->
	 * BF_EFORMAT */
	bf_wav_chunk_t other_chunk;
	memcpy(other_chunk.id, "JUNK", 4U);
	other_chunk.id[4] = '\0';
	other_chunk.size = 4U;
	other_chunk.data = bad_fmt_payload;
	CHECK(bf_wav_parse_fmt(&other_chunk, &fmt) == BF_EFORMAT);
	CHECK(bf_wav_get_data(&other_chunk, &data) == BF_EFORMAT);

	/* Truncated chunk payload -> BF_EOF without advancing stream */
	CHECK(bf_stream_init(&s, wav, len - 3U) == BF_OK);
	CHECK(bf_wav_read_riff_header(&s, &rhdr) == BF_OK);
	bf_wav_chunk_t c;
	CHECK(bf_wav_read_chunk(&s, &c) == BF_OK); /* fmt */
	CHECK(bf_wav_read_chunk(&s, &c) == BF_OK); /* PAD */
	size_t before_data = s.byte_offset;
	CHECK(bf_wav_read_chunk(&s, &c) == BF_EOF);
	CHECK(s.byte_offset == before_data);

	/* Invalid / NULL arguments -> BF_EINVAL */
	CHECK(bf_wav_read_chunk(NULL, &c) == BF_EINVAL);
	CHECK(bf_wav_read_chunk(&s, NULL) == BF_EINVAL);
	CHECK(bf_wav_parse_fmt(NULL, &fmt) == BF_EINVAL);
	CHECK(bf_wav_parse_fmt(&bad_fmt_chunk, NULL) == BF_EINVAL);
	CHECK(bf_wav_get_data(NULL, &data) == BF_EINVAL);
	CHECK(bf_wav_get_data(&bad_fmt_chunk, NULL) == BF_EINVAL);
	CHECK(!bf_wav_chunk_is(NULL, "fmt "));

	printf("test_wav_chunks_fmt_and_data finished.\n");
}

int main(void)
{
	test_wav_riff_header_and_errors();
	test_wav_chunks_fmt_and_data();

	if (failures != 0) {
		fprintf(stderr, "%d check(s) failed\n", failures);
		return EXIT_FAILURE;
	}

	printf("All WAV parser tests passed.\n");
	return EXIT_SUCCESS;
}
