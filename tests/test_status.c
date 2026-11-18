#include <bitforge/status.h>

#include <stdio.h>
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

static void test_status_strings(void)
{
	CHECK(strcmp(bf_status_string(BF_OK), "Success") == 0);
	CHECK(strcmp(bf_status_string(BF_EINVAL), "Invalid argument") == 0);
	CHECK(strcmp(bf_status_string(BF_ENOMEM), "Out of memory") == 0);
	CHECK(strcmp(bf_status_string(BF_EIO), "Input/output error") == 0);
	CHECK(strcmp(bf_status_string(BF_EOF), "End of input") == 0);
	CHECK(strcmp(bf_status_string(BF_ENOTFOUND), "Not found") == 0);
	CHECK(
	    strcmp(bf_status_string(BF_EOVERFLOW), "Arithmetic overflow") == 0);
	CHECK(strcmp(bf_status_string(BF_EUNSUPPORTED),
		  "Operation not supported") == 0);
	CHECK(strcmp(bf_status_string(BF_EFORMAT), "Invalid data format") == 0);
}

static void test_unknown_status(void)
{
	const char *message = bf_status_string((bf_status)-1);

	CHECK(message != NULL);
	CHECK(strcmp(message, "Unknown status") == 0);
}

int main(void)
{
	test_status_strings();
	test_unknown_status();

	if (failures > 0) {
		fprintf(stderr, "%d test(s) failed.\n", failures);
		return 1;
	}

	printf("All status tests passed.\n");
	return 0;
}
