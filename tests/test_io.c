#include <bitforge/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

#define TEST_FILE "bitforge_test_io.tmp"

static const uint8_t test_data[] = {0x10, 0x20, 0x30, 0x40, 0x50};

static void create_test_file(void)
{
	FILE *file = fopen(TEST_FILE, "wb");

	CHECK(file != NULL);

	if (file == NULL) {
		return;
	}

	CHECK(
	    fwrite(test_data, 1, sizeof(test_data), file) == sizeof(test_data));

	CHECK(fclose(file) == 0);
}

static void remove_test_file(void)
{
	(void)remove(TEST_FILE);
}

static void test_io_open(void)
{
	bf_file_t file = {0};

	remove_test_file();

	CHECK(bf_io_open(NULL, TEST_FILE, BF_FILE_READ) == BF_EINVAL);
	CHECK(bf_io_open(&file, NULL, BF_FILE_READ) == BF_EINVAL);
	CHECK(bf_io_open(&file, "", BF_FILE_READ) == BF_EINVAL);
	CHECK(bf_io_open(&file, TEST_FILE, (bf_file_mode)-1) == BF_EINVAL);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);
	CHECK(bf_io_is_open(&file));

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_EINVAL);

	CHECK(bf_io_close(&file) == BF_OK);

	CHECK(bf_io_open(&file, "nonexistent_bitforge_file.tmp",
		  BF_FILE_READ) == BF_EIO);

	CHECK(!bf_io_is_open(&file));

	remove_test_file();
}

static void test_io_close(void)
{
	bf_file_t file = {0};

	CHECK(bf_io_close(NULL) == BF_EINVAL);
	CHECK(bf_io_close(&file) == BF_EINVAL);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_close(&file) == BF_OK);
	CHECK(file.handle == NULL);

	CHECK(bf_io_close(&file) == BF_EINVAL);

	remove_test_file();
}

static void test_io_is_open(void)
{
	bf_file_t file = {0};

	CHECK(!bf_io_is_open(NULL));
	CHECK(!bf_io_is_open(&file));

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);
	CHECK(bf_io_is_open(&file));

	CHECK(bf_io_close(&file) == BF_OK);
	CHECK(!bf_io_is_open(&file));

	remove_test_file();
}

static void test_io_read(void)
{
	bf_file_t file = {0};
	uint8_t buffer[sizeof(test_data)] = {0};
	size_t bytes_read = 0;

	CHECK(
	    bf_io_read(NULL, buffer, sizeof(buffer), &bytes_read) == BF_EINVAL);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) ==
	      BF_EINVAL);

	CHECK(
	    bf_io_read(&file, NULL, sizeof(buffer), &bytes_read) == BF_EINVAL);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), NULL) == BF_EINVAL);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);

	CHECK(bytes_read == sizeof(test_data));
	CHECK(memcmp(buffer, test_data, sizeof(test_data)) == 0);

	bytes_read = 123;

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_EOF);
	CHECK(bytes_read == 0);

	CHECK(bf_io_read(&file, NULL, 0, &bytes_read) == BF_OK);
	CHECK(bytes_read == 0);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_read_partial(void)
{
	bf_file_t file = {0};
	uint8_t buffer[10] = {0};
	size_t bytes_read = 0;

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);

	CHECK(bytes_read == sizeof(test_data));
	CHECK(memcmp(buffer, test_data, sizeof(test_data)) == 0);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_read_modes(void)
{
	bf_file_t file = {0};
	uint8_t buffer[sizeof(test_data)] = {0};
	size_t bytes_read = 0;

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_WRITE) == BF_OK);
	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) ==
	      BF_EINVAL);
	CHECK(bytes_read == 0);
	CHECK(bf_io_close(&file) == BF_OK);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_APPEND) == BF_OK);
	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) ==
	      BF_EINVAL);
	CHECK(bytes_read == 0);
	CHECK(bf_io_close(&file) == BF_OK);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ_WRITE) == BF_OK);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);
	CHECK(bytes_read == sizeof(test_data));
	CHECK(memcmp(buffer, test_data, sizeof(test_data)) == 0);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_write(void)
{
	bf_file_t file = {0};
	const uint8_t data[] = {0xAA, 0xBB, 0xCC};
	uint8_t buffer[sizeof(data)] = {0};
	size_t bytes_written = 0;
	size_t bytes_read = 0;

	CHECK(
	    bf_io_write(NULL, data, sizeof(data), &bytes_written) == BF_EINVAL);

	CHECK(bf_io_write(&file, data, sizeof(data), &bytes_written) ==
	      BF_EINVAL);

	CHECK(bf_io_write(&file, NULL, sizeof(data), &bytes_written) ==
	      BF_EINVAL);

	CHECK(bf_io_write(&file, data, sizeof(data), NULL) == BF_EINVAL);

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_WRITE) == BF_OK);

	CHECK(bf_io_write(&file, data, sizeof(data), &bytes_written) == BF_OK);
	CHECK(bytes_written == sizeof(data));

	CHECK(bf_io_write(&file, NULL, 0, &bytes_written) == BF_OK);
	CHECK(bytes_written == 0);

	CHECK(bf_io_close(&file) == BF_OK);

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);
	CHECK(bytes_read == sizeof(data));
	CHECK(memcmp(buffer, data, sizeof(data)) == 0);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_write_modes(void)
{
	bf_file_t file = {0};
	const uint8_t data[] = {0xAA, 0xBB};
	size_t bytes_written = 0;

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);
	CHECK(bf_io_write(&file, data, sizeof(data), &bytes_written) ==
	      BF_EINVAL);
	CHECK(bytes_written == 0);
	CHECK(bf_io_close(&file) == BF_OK);

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_APPEND) == BF_OK);
	CHECK(bf_io_write(&file, data, sizeof(data), &bytes_written) == BF_OK);
	CHECK(bytes_written == sizeof(data));
	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_append(void)
{
	bf_file_t file = {0};
	const uint8_t appended[] = {0xAA, 0xBB};
	uint8_t buffer[sizeof(test_data) + sizeof(appended)] = {0};
	size_t bytes_written = 0;
	size_t bytes_read = 0;

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_APPEND) == BF_OK);
	CHECK(bf_io_write(&file, appended, sizeof(appended), &bytes_written) ==
	      BF_OK);
	CHECK(bytes_written == sizeof(appended));
	CHECK(bf_io_close(&file) == BF_OK);

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);
	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);
	CHECK(bytes_read == sizeof(buffer));
	CHECK(memcmp(buffer, test_data, sizeof(test_data)) == 0);
	CHECK(memcmp(buffer + sizeof(test_data), appended, sizeof(appended)) ==
	      0);
	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_read_write_transitions(void)
{
	bf_file_t file = {0};
	const uint8_t replacement[] = {0xAA, 0xBB};
	const uint8_t expected[] = {0x10, 0x20, 0xAA, 0xBB, 0x50};
	uint8_t buffer[sizeof(test_data)] = {0};
	uint8_t first_bytes[2] = {0};
	uint8_t last_byte = 0;
	size_t bytes_read = 0;
	size_t bytes_written = 0;

	create_test_file();
	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ_WRITE) == BF_OK);

	CHECK(bf_io_read(&file, first_bytes, sizeof(first_bytes),
		  &bytes_read) == BF_OK);
	CHECK(bytes_read == sizeof(first_bytes));
	CHECK(memcmp(first_bytes, test_data, sizeof(first_bytes)) == 0);

	CHECK(bf_io_write(&file, replacement, sizeof(replacement),
		  &bytes_written) == BF_OK);
	CHECK(bytes_written == sizeof(replacement));

	CHECK(bf_io_read(&file, &last_byte, 1, &bytes_read) == BF_OK);
	CHECK(bytes_read == 1);
	CHECK(last_byte == 0x50);

	CHECK(bf_io_seek(&file, 0) == BF_OK);
	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);
	CHECK(bytes_read == sizeof(buffer));
	CHECK(memcmp(buffer, expected, sizeof(expected)) == 0);

	CHECK(bf_io_close(&file) == BF_OK);
	remove_test_file();
}

static void test_io_seek(void)
{
	bf_file_t file = {0};
	uint8_t byte = 0;
	size_t bytes_read = 0;

	create_test_file();

	CHECK(bf_io_seek(NULL, 0) == BF_EINVAL);

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_seek(&file, 2) == BF_OK);
	CHECK(bf_io_read(&file, &byte, 1, &bytes_read) == BF_OK);
	CHECK(bytes_read == 1);
	CHECK(byte == test_data[2]);

	CHECK(bf_io_seek(&file, 0) == BF_OK);
	CHECK(bf_io_read(&file, &byte, 1, &bytes_read) == BF_OK);
	CHECK(byte == test_data[0]);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_seek_overflow(void)
{
	bf_file_t file = {0};

	create_test_file();
	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

#if SIZE_MAX > LONG_MAX
	CHECK(bf_io_seek(&file, (size_t)LONG_MAX + 1u) == BF_EOVERFLOW);
#endif

	CHECK(bf_io_close(&file) == BF_OK);
	remove_test_file();
}

static void test_io_tell(void)
{
	bf_file_t file = {0};
	uint8_t buffer[2];
	size_t position = 0;
	size_t bytes_read = 0;

	CHECK(bf_io_tell(NULL, &position) == BF_EINVAL);
	CHECK(bf_io_tell(&file, &position) == BF_EINVAL);
	CHECK(bf_io_tell(&file, NULL) == BF_EINVAL);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_tell(&file, &position) == BF_OK);
	CHECK(position == 0);

	CHECK(bf_io_read(&file, buffer, sizeof(buffer), &bytes_read) == BF_OK);

	CHECK(bf_io_tell(&file, &position) == BF_OK);
	CHECK(position == 2);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

static void test_io_size(void)
{
	bf_file_t file = {0};
	size_t file_size = 0;
	size_t position_before = 0;
	size_t position_after = 0;
	uint8_t byte;
	size_t bytes_read = 0;

	CHECK(bf_io_size(NULL, &file_size) == BF_EINVAL);
	CHECK(bf_io_size(&file, &file_size) == BF_EINVAL);
	CHECK(bf_io_size(&file, NULL) == BF_EINVAL);

	create_test_file();

	CHECK(bf_io_open(&file, TEST_FILE, BF_FILE_READ) == BF_OK);

	CHECK(bf_io_read(&file, &byte, 1, &bytes_read) == BF_OK);

	CHECK(bf_io_tell(&file, &position_before) == BF_OK);
	CHECK(bf_io_size(&file, &file_size) == BF_OK);

	CHECK(file_size == sizeof(test_data));

	CHECK(bf_io_tell(&file, &position_after) == BF_OK);
	CHECK(position_before == position_after);

	CHECK(bf_io_close(&file) == BF_OK);

	remove_test_file();
}

int main(void)
{
	test_io_open();
	test_io_close();
	test_io_is_open();
	test_io_read();
	test_io_read_partial();
	test_io_read_modes();
	test_io_write();
	test_io_write_modes();
	test_io_append();
	test_io_read_write_transitions();
	test_io_seek();
	test_io_seek_overflow();
	test_io_tell();
	test_io_size();

	remove_test_file();

	if (failures > 0) {
		fprintf(stderr, "\n%d test(s) failed.\n", failures);
		return EXIT_FAILURE;
	}

	printf("All IO tests passed.\n");

	return EXIT_SUCCESS;
}
