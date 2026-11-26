#include <bitforge/bitops.h>
#include <bitforge/buffer.h>
#include <bitforge/formats/bmp.h>
#include <bitforge/formats/elf.h>
#include <bitforge/formats/pe.h>
#include <bitforge/formats/png.h>
#include <bitforge/formats/wav.h>
#include <bitforge/io.h>
#include <bitforge/parser.h>
#include <bitforge/pattern.h>
#include <bitforge/status.h>
#include <bitforge/stream.h>
#include <stdint.h>
#include <stdio.h>

int main(void)
{
	uint8_t storage[16] = {0};
	bf_buffer_t buffer;

	if (bf_buffer_init(&buffer, storage, sizeof(storage)) != BF_OK) {
		return 1;
	}

	const uint8_t payload[4] = {0x10, 0x20, 0x30, 0x40};
	if (bf_buffer_append(&buffer, payload, sizeof(payload)) != BF_OK) {
		return 1;
	}

	if (buffer.size != sizeof(payload)) {
		return 1;
	}

	if (bf_status_string(BF_OK) == NULL) {
		return 1;
	}

	puts("External test passed");
	return 0;
}
