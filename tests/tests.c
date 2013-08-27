#include <stdio.h>
#include <string.h>

#include "test_framework.h"
#include "test_functions.c"
#include <utils.h>

static struct test tests[] =
			{TEST_LIST(TEST_STRUCT_ENTRY) /*,*/ {"NULL", NULL}};

static void __print_test_header(struct test *t, int num, int max)
{
	printf("%d/%d: " FORMAT_UNDERLINE(FORMAT_BLUE("%s")) "\n", num, max, t->name);
}

static int __print_test_result(struct test *t)
{
	int result = t->func();

	printf("RESULT: %s\n", (result == TEST_SUCCESS) ? FORMAT_GREEN(S(TEST_SUCCESS)) : FORMAT_RED(S(TEST_FAILURE)));

	return result;	
}

static void __print_test_footer(void)
{
	putchar('\n');
}

test_error run_tests(void)
{
	struct test *t = &tests[0];
	int i = 1;
	test_error result = TEST_SUCCESS;
	int max_tests = array_length(tests) - 1;

	printf(FORMAT_BOLD(FORMAT_YELLOW("Testing framework: ")) "Running %d tests.\n\n", max_tests);

	for (; t->func != NULL; ++t, ++i)
	{
		__print_test_header(t, i, max_tests);

		if (__print_test_result(t) == TEST_FAILURE)
			result = TEST_FAILURE;
		
		__print_test_footer();
	}

	return result;
}