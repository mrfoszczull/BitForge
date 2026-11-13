#include <bitforge/buffer.h>
#include <stdio.h>
#include <stdbool.h>

static int failures = 0;

#define CHECK(cond)                                                            \
	do {                                                                   \
		if (!(cond)) {                                                 \
			fprintf(stderr, "%s:%d: CHECK failed: %s\n", __FILE__, \
			    __LINE__, #cond);                                  \
			failures++;                                            \
		}                                                              \
	} while (0)

/* bf_view_init tests */

static void test_view_init_valid(void)
{
	uint8_t data[] = {1, 2, 3, 4};
	bf_view_t view;

	CHECK(bf_view_init(&view, data, sizeof(data)) == BF_OK);
	CHECK(view.data == data);
	CHECK(view.size == sizeof(data));
}

static void test_view_init_empty(void)
{
	bf_view_t view;

	CHECK(bf_view_init(&view, NULL, 0) == BF_OK);
	CHECK(view.data == NULL);
	CHECK(view.size == 0);
}

static void test_view_init_null_destination(void)
{
	uint8_t data[] = {1, 2, 3};

	CHECK(bf_view_init(NULL, data, sizeof(data)) == BF_EINVAL);
}

static void test_view_init_null_data(void)
{
	bf_view_t view;

	CHECK(bf_view_init(&view, NULL, 4) == BF_EINVAL);
}

/* bf_buffer_init tests */

static void test_buffer_init_valid(void)
{
	uint8_t data[16];
	bf_buffer_t buffer;

	CHECK(bf_buffer_init(&buffer, data, sizeof(data)) == BF_OK);
	CHECK(buffer.data == data);
	CHECK(buffer.capacity == sizeof(data));
	CHECK(buffer.size == 0);
}

static void test_buffer_init_empty(void)
{
	bf_buffer_t buffer;

	CHECK(bf_buffer_init(&buffer, NULL, 0) == BF_OK);
	CHECK(buffer.data == NULL);
	CHECK(buffer.capacity == 0);
	CHECK(buffer.size == 0);
}

static void test_buffer_init_null_destination(void)
{
	uint8_t data[16];

	CHECK(bf_buffer_init(NULL, data, sizeof(data)) == BF_EINVAL);
}

static void test_buffer_init_null_data(void)
{
	bf_buffer_t buffer;

	CHECK(bf_buffer_init(&buffer, NULL, 16) == BF_EINVAL);
}

static void test_bf_view_read(void)
{
	const uint8_t source[] = {10, 20, 30, 40, 50};
	uint8_t destination[3] = {0};

	bf_view_t view = {.data = source, .size = sizeof(source)};

	CHECK(bf_view_read(&view, 1, destination, 3) == BF_OK);
	CHECK(destination[0] == 20);
	CHECK(destination[1] == 30);
	CHECK(destination[2] == 40);

	CHECK(bf_view_read(&view, 0, destination, 3) == BF_OK);
	CHECK(destination[0] == 10);

	CHECK(bf_view_read(&view, 5, NULL, 0) == BF_OK);

	CHECK(bf_view_read(NULL, 0, destination, 1) == BF_EINVAL);
	CHECK(bf_view_read(&view, 0, NULL, 1) == BF_EINVAL);

	CHECK(bf_view_read(&view, 4, destination, 2) == BF_EINVAL);
	CHECK(bf_view_read(&view, 6, destination, 0) == BF_EINVAL);

	bf_view_t invalid_view = {.data = NULL, .size = 5};

	CHECK(bf_view_read(&invalid_view, 0, destination, 1) == BF_EINVAL);
}

static void test_bf_view_slice(void)
{
	const uint8_t source[] = {10, 20, 30, 40, 50};

	bf_view_t view = {.data = source, .size = sizeof(source)};

	bf_view_t result = {0};

	CHECK(bf_view_slice(&view, 1, 3, &result) == BF_OK);
	CHECK(result.data == source + 1);
	CHECK(result.size == 3);
	CHECK(result.data[0] == 20);
	CHECK(result.data[2] == 40);

	CHECK(bf_view_slice(&view, 5, 0, &result) == BF_OK);
	CHECK(result.data == source + 5);
	CHECK(result.size == 0);

	bf_view_t empty_view = {.data = NULL, .size = 0};

	CHECK(bf_view_slice(&empty_view, 0, 0, &result) == BF_OK);
	CHECK(result.data == NULL);
	CHECK(result.size == 0);

	CHECK(bf_view_slice(NULL, 0, 1, &result) == BF_EINVAL);
	CHECK(bf_view_slice(&view, 0, 1, NULL) == BF_EINVAL);

	CHECK(bf_view_slice(&view, 4, 2, &result) == BF_EINVAL);
	CHECK(bf_view_slice(&view, 6, 0, &result) == BF_EINVAL);

	bf_view_t invalid_view = {.data = NULL, .size = 5};

	result.data = source;
	result.size = 2;

	CHECK(bf_view_slice(&invalid_view, 0, 1, &result) == BF_EINVAL);
	CHECK(result.data == source);
	CHECK(result.size == 2);

	bf_view_t alias = {.data = source, .size = sizeof(source)};

	CHECK(bf_view_slice(&alias, 2, 2, &alias) == BF_OK);
	CHECK(alias.data == source + 2);
	CHECK(alias.size == 2);
}

static void test_bf_buffer_write(void)
{
	uint8_t memory[5] = {0};
	const uint8_t source[] = {10, 20, 30};

	bf_buffer_t buffer = {
	    .data = memory, .capacity = sizeof(memory), .size = 3};

	CHECK(bf_buffer_write(&buffer, 1, source, 3) == BF_OK);
	CHECK(memory[0] == 0);
	CHECK(memory[1] == 10);
	CHECK(memory[2] == 20);
	CHECK(memory[3] == 30);

	CHECK(buffer.size == 3);

	CHECK(bf_buffer_write(&buffer, 5, NULL, 0) == BF_OK);

	CHECK(bf_buffer_write(NULL, 0, source, 1) == BF_EINVAL);
	CHECK(bf_buffer_write(&buffer, 0, NULL, 1) == BF_EINVAL);

	CHECK(bf_buffer_write(&buffer, 4, source, 2) == BF_EINVAL);
	CHECK(bf_buffer_write(&buffer, 6, source, 0) == BF_EINVAL);

	bf_buffer_t invalid_size = {.data = memory, .capacity = 5, .size = 6};

	CHECK(bf_buffer_write(&invalid_size, 0, source, 1) == BF_EINVAL);

	bf_buffer_t invalid_data = {.data = NULL, .capacity = 5, .size = 0};

	CHECK(bf_buffer_write(&invalid_data, 0, source, 1) == BF_EINVAL);
}

static void test_bf_buffer_get_view(void)
{
	uint8_t memory[] = {10, 20, 30};

	bf_buffer_t buffer = {
	    .data = memory, .capacity = sizeof(memory), .size = sizeof(memory)};

	bf_view_t view = {0};

	CHECK(bf_buffer_get_view(&buffer, &view) == BF_OK);
	CHECK(view.data == memory);
	CHECK(view.size == 3);

	bf_buffer_t empty_buffer = {.data = NULL, .capacity = 0, .size = 0};

	CHECK(bf_buffer_get_view(&empty_buffer, &view) == BF_OK);
	CHECK(view.data == NULL);
	CHECK(view.size == 0);

	CHECK(bf_buffer_get_view(NULL, &view) == BF_EINVAL);
	CHECK(bf_buffer_get_view(&buffer, NULL) == BF_EINVAL);

	bf_buffer_t invalid_size = {.data = memory, .capacity = 2, .size = 3};

	view.data = memory;
	view.size = 2;

	CHECK(bf_buffer_get_view(&invalid_size, &view) == BF_EINVAL);
	CHECK(view.data == memory);
	CHECK(view.size == 2);

	bf_buffer_t invalid_data = {.data = NULL, .capacity = 3, .size = 0};

	CHECK(bf_buffer_get_view(&invalid_data, &view) == BF_EINVAL);
	CHECK(view.data == memory);
	CHECK(view.size == 2);
}

static void test_bf_buffer_append(void)
{
	uint8_t memory[5] = {0};
	const uint8_t first[] = {10, 20};
	const uint8_t second[] = {30, 40, 50};
	const uint8_t extra[] = {60};

	bf_buffer_t buffer = {
	    .data = memory, .capacity = sizeof(memory), .size = 0};

	CHECK(bf_buffer_append(&buffer, first, 2) == BF_OK);
	CHECK(buffer.size == 2);
	CHECK(memory[0] == 10);
	CHECK(memory[1] == 20);

	CHECK(bf_buffer_append(&buffer, second, 3) == BF_OK);
	CHECK(buffer.size == 5);
	CHECK(memory[0] == 10);
	CHECK(memory[1] == 20);
	CHECK(memory[2] == 30);
	CHECK(memory[3] == 40);
	CHECK(memory[4] == 50);

	CHECK(bf_buffer_append(&buffer, extra, 1) == BF_EINVAL);
	CHECK(buffer.size == 5);
	CHECK(memory[4] == 50);

	CHECK(bf_buffer_append(&buffer, NULL, 1) == BF_EINVAL);
	CHECK(buffer.size == 5);

	CHECK(bf_buffer_append(&buffer, NULL, 0) == BF_OK);
	CHECK(buffer.size == 5);

	bf_buffer_t invalid_size = {.data = memory, .capacity = 5, .size = 6};

	CHECK(bf_buffer_append(&invalid_size, extra, 1) == BF_EINVAL);
	CHECK(invalid_size.size == 6);

	bf_buffer_t invalid_data = {.data = NULL, .capacity = 5, .size = 0};

	CHECK(bf_buffer_append(&invalid_data, extra, 1) == BF_EINVAL);
	CHECK(invalid_data.size == 0);

	uint8_t empty_memory[3] = {0};

	bf_buffer_t empty_buffer = {
	    .data = empty_memory, .capacity = sizeof(empty_memory), .size = 0};

	CHECK(bf_buffer_append(&empty_buffer, first, 0) == BF_OK);
	CHECK(empty_buffer.size == 0);
}

static void test_bf_buffer_clear(void)
{
	uint8_t memory[] = {10, 20, 30, 40};

	bf_buffer_t buffer = {
	    .data = memory, .capacity = sizeof(memory), .size = 4};

	CHECK(bf_buffer_clear(&buffer) == BF_OK);
	CHECK(buffer.size == 0);
	CHECK(buffer.capacity == 4);
	CHECK(buffer.data == memory);

	CHECK(memory[0] == 10);
	CHECK(memory[1] == 20);
	CHECK(memory[2] == 30);
	CHECK(memory[3] == 40);

	CHECK(bf_buffer_clear(&buffer) == BF_OK);
	CHECK(buffer.size == 0);

	CHECK(bf_buffer_clear(NULL) == BF_EINVAL);

	bf_buffer_t invalid_size = {.data = memory, .capacity = 4, .size = 5};

	CHECK(bf_buffer_clear(&invalid_size) == BF_EINVAL);
	CHECK(invalid_size.size == 5);

	bf_buffer_t invalid_data = {.data = NULL, .capacity = 4, .size = 0};

	CHECK(bf_buffer_clear(&invalid_data) == BF_EINVAL);
	CHECK(invalid_data.size == 0);

	bf_buffer_t empty_buffer = {.data = NULL, .capacity = 0, .size = 0};

	CHECK(bf_buffer_clear(&empty_buffer) == BF_OK);
	CHECK(empty_buffer.size == 0);
}

static void test_bf_buffer_fill(void)
{
	uint8_t memory[6] = {0};

	bf_buffer_t buffer = {
	    .data = memory, .capacity = sizeof(memory), .size = 4};

	CHECK(bf_buffer_fill(&buffer, 0, 6, 0xFF) == BF_OK);

	for (size_t i = 0; i < 6; i++) {
		CHECK(memory[i] == 0xFF);
	}

	CHECK(buffer.size == 4);
	CHECK(buffer.capacity == 6);

	CHECK(bf_buffer_fill(&buffer, 1, 3, 0xAA) == BF_OK);
	CHECK(memory[0] == 0xFF);
	CHECK(memory[1] == 0xAA);
	CHECK(memory[2] == 0xAA);
	CHECK(memory[3] == 0xAA);
	CHECK(memory[4] == 0xFF);
	CHECK(memory[5] == 0xFF);

	CHECK(buffer.size == 4);

	CHECK(bf_buffer_fill(&buffer, 0, 2, 0x00) == BF_OK);
	CHECK(memory[0] == 0x00);
	CHECK(memory[1] == 0x00);

	CHECK(bf_buffer_fill(&buffer, 6, 0, 0xFF) == BF_OK);
	CHECK(bf_buffer_fill(&buffer, 7, 0, 0xFF) == BF_EINVAL);
	CHECK(bf_buffer_fill(&buffer, 5, 2, 0xFF) == BF_EINVAL);

	CHECK(bf_buffer_fill(NULL, 0, 1, 0xFF) == BF_EINVAL);

	bf_buffer_t invalid_size = {.data = memory, .capacity = 6, .size = 7};

	CHECK(bf_buffer_fill(&invalid_size, 0, 1, 0xFF) == BF_EINVAL);
	CHECK(invalid_size.size == 7);

	bf_buffer_t invalid_data = {.data = NULL, .capacity = 6, .size = 0};

	CHECK(bf_buffer_fill(&invalid_data, 0, 1, 0xFF) == BF_EINVAL);

	bf_buffer_t empty_buffer = {.data = NULL, .capacity = 0, .size = 0};

	CHECK(bf_buffer_fill(&empty_buffer, 0, 0, 0xFF) == BF_OK);
}

static void test_bf_view_compare(void)
{
	const uint8_t data_a[] = {10, 20, 30, 40};
	const uint8_t data_b[] = {10, 20, 30, 40};
	const uint8_t data_c[] = {10, 20, 30, 50};
	const uint8_t data_d[] = {10, 20, 30};

	bf_view_t view_a = {.data = data_a, .size = sizeof(data_a)};

	bf_view_t view_b = {.data = data_b, .size = sizeof(data_b)};

	bf_view_t view_c = {.data = data_c, .size = sizeof(data_c)};

	bf_view_t view_d = {.data = data_d, .size = sizeof(data_d)};

	bool equal = false;

	CHECK(bf_view_compare(&view_a, &view_b, &equal) == BF_OK);
	CHECK(equal == true);

	CHECK(bf_view_compare(&view_a, &view_c, &equal) == BF_OK);
	CHECK(equal == false);

	CHECK(bf_view_compare(&view_a, &view_d, &equal) == BF_OK);
	CHECK(equal == false);

	CHECK(bf_view_compare(&view_a, &view_a, &equal) == BF_OK);
	CHECK(equal == true);

	bf_view_t empty_a = {.data = NULL, .size = 0};

	bf_view_t empty_b = {.data = NULL, .size = 0};

	CHECK(bf_view_compare(&empty_a, &empty_b, &equal) == BF_OK);
	CHECK(equal == true);

	CHECK(bf_view_compare(NULL, &view_a, &equal) == BF_EINVAL);
	CHECK(bf_view_compare(&view_a, NULL, &equal) == BF_EINVAL);
	CHECK(bf_view_compare(&view_a, &view_b, NULL) == BF_EINVAL);

	bf_view_t invalid_view = {.data = NULL, .size = 4};

	equal = true;
	CHECK(bf_view_compare(&invalid_view, &view_a, &equal) == BF_EINVAL);
	CHECK(equal == true);

	equal = true;
	CHECK(bf_view_compare(&view_a, &invalid_view, &equal) == BF_EINVAL);
	CHECK(equal == true);
}

static void test_bf_buffer_copy(void)
{
	uint8_t source_memory[] = {10, 20, 30, 40};
	uint8_t destination_memory[6] = {0};

	bf_buffer_t source = {.data = source_memory,
	    .capacity = sizeof(source_memory),
	    .size = sizeof(source_memory)};

	bf_buffer_t destination = {.data = destination_memory,
	    .capacity = sizeof(destination_memory),
	    .size = 2};

	CHECK(bf_buffer_copy(&destination, &source) == BF_OK);
	CHECK(destination.size == 4);
	CHECK(destination.capacity == 6);
	CHECK(destination.data == destination_memory);

	CHECK(destination_memory[0] == 10);
	CHECK(destination_memory[1] == 20);
	CHECK(destination_memory[2] == 30);
	CHECK(destination_memory[3] == 40);

	CHECK(bf_buffer_copy(&destination, &source) == BF_OK);
	CHECK(destination.size == 4);

	uint8_t small_memory[2] = {50, 60};

	bf_buffer_t small_destination = {
	    .data = small_memory, .capacity = sizeof(small_memory), .size = 2};

	CHECK(bf_buffer_copy(&small_destination, &source) == BF_EINVAL);
	CHECK(small_destination.size == 2);
	CHECK(small_memory[0] == 50);
	CHECK(small_memory[1] == 60);

	CHECK(bf_buffer_copy(NULL, &source) == BF_EINVAL);
	CHECK(bf_buffer_copy(&destination, NULL) == BF_EINVAL);

	bf_buffer_t invalid_source_size = {
	    .data = source_memory, .capacity = 4, .size = 5};

	CHECK(bf_buffer_copy(&destination, &invalid_source_size) == BF_EINVAL);
	CHECK(destination.size == 4);
	CHECK(destination_memory[0] == 10);

	bf_buffer_t invalid_source_data = {
	    .data = NULL, .capacity = 4, .size = 1};

	CHECK(bf_buffer_copy(&destination, &invalid_source_data) == BF_EINVAL);
	CHECK(destination.size == 4);

	bf_buffer_t invalid_destination_size = {
	    .data = destination_memory, .capacity = 6, .size = 7};

	CHECK(bf_buffer_copy(&invalid_destination_size, &source) == BF_EINVAL);
	CHECK(invalid_destination_size.size == 7);

	bf_buffer_t invalid_destination_data = {
	    .data = NULL, .capacity = 6, .size = 0};

	CHECK(bf_buffer_copy(&invalid_destination_data, &source) == BF_EINVAL);
	CHECK(invalid_destination_data.size == 0);

	bf_buffer_t empty_source = {.data = NULL, .capacity = 0, .size = 0};

	bf_buffer_t empty_destination = {
	    .data = NULL, .capacity = 0, .size = 0};

	CHECK(bf_buffer_copy(&empty_destination, &empty_source) == BF_OK);
	CHECK(empty_destination.size == 0);
}

static void test_bf_buffer_read(void)
{
	uint8_t memory[] = {10, 20, 30, 40, 50};
	uint8_t destination[3] = {0};

	bf_buffer_t buffer = {
	    .data = memory, .capacity = sizeof(memory), .size = 4};

	CHECK(bf_buffer_read(&buffer, 1, destination, 3) == BF_OK);
	CHECK(destination[0] == 20);
	CHECK(destination[1] == 30);
	CHECK(destination[2] == 40);

	CHECK(bf_buffer_read(&buffer, 0, destination, 3) == BF_OK);
	CHECK(destination[0] == 10);
	CHECK(destination[1] == 20);
	CHECK(destination[2] == 30);

	CHECK(bf_buffer_read(&buffer, 2, NULL, 0) == BF_OK);

	CHECK(bf_buffer_read(NULL, 0, destination, 1) == BF_EINVAL);
	CHECK(bf_buffer_read(&buffer, 0, NULL, 1) == BF_EINVAL);

	CHECK(bf_buffer_read(&buffer, 3, destination, 2) ==
	      BF_EINVAL); // 3 + 2 = 5 > 4
	CHECK(bf_buffer_read(&buffer, 5, destination, 0) ==
	      BF_EINVAL); // offset > size

	bf_buffer_t invalid_size = {.data = memory, .capacity = 5, .size = 6};
	CHECK(bf_buffer_read(&invalid_size, 0, destination, 1) == BF_EINVAL);

	bf_buffer_t invalid_data = {.data = NULL, .capacity = 5, .size = 0};
	CHECK(bf_buffer_read(&invalid_data, 0, destination, 1) == BF_EINVAL);
}

int main(void)
{
	test_view_init_valid();
	test_view_init_empty();
	test_view_init_null_destination();
	test_view_init_null_data();

	test_buffer_init_valid();
	test_buffer_init_empty();
	test_buffer_init_null_destination();
	test_buffer_init_null_data();

	test_bf_view_read();
	test_bf_view_slice();
	test_bf_buffer_write();
	test_bf_buffer_get_view();

	test_bf_buffer_append();
	test_bf_buffer_clear();
	test_bf_buffer_fill();
	test_bf_view_compare();

	test_bf_buffer_copy();
	test_bf_buffer_read();

	if (failures != 0) {
		fprintf(stderr, "%d test(s) failed.\n", failures);
		return 1;
	}

	puts("All buffer tests passed.");
	return 0;
}
