#include <bitforge/buffer.h>
#include <stdio.h>

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

	if (failures != 0) {
		fprintf(stderr, "%d test(s) failed.\n", failures);
		return 1;
	}

	puts("All buffer tests passed.");
	return 0;
}
