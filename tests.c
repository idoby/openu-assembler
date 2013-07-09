#ifdef RUN_TESTS

#include <stdio.h>
#include <string.h>

#include "test_framework.h"

#include "test_functions.c"

struct test tests[] =
			{	TEST_LIST(TEST_STRUCT_ENTRY)
				{"NULL", NULL}			};

static void __print_test_header(struct test *t, int num, int max)
{
	printf(/*"RUNNING TEST "*/ "%d/%d: " FORMAT_UNDERLINE(FORMAT_BLUE("%s")) "\n", num, max, t->name);
	/*printf("=============");	
	{
		int i, len = strlen(t->name);
		for (i = 0; i < len; ++i)
			putchar('=');
	}*/
}

static void __print_test_result(struct test *t)
{
	printf("RESULT: %s\n", (t->func() == TEST_SUCCESS) ? FORMAT_GREEN(S(TEST_SUCCESS)) : FORMAT_RED(S(TEST_FAILURE)));
}

static void __print_test_footer(void)
{
	putchar('\n');
}

#endif /* RUN_TESTS */

void run_tests(void)
{
#ifdef RUN_TESTS
	struct test *t = &tests[0];
	int i = 1;
	int max_tests = sizeof(tests) / sizeof(tests[0]) - 1;

	printf(FORMAT_BOLD(FORMAT_YELLOW("Testing framework: ")) "Running %d tests.\n\n", max_tests);

	for (; t->func != NULL; ++t, ++i)
	{
		__print_test_header(t, i, max_tests);
		__print_test_result(t);
		__print_test_footer();
	}
#endif /* RUN_TESTS */
}