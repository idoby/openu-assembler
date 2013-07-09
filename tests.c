#ifdef RUN_TESTS

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "symbol_table.h"
#include "assembler.h"

#include "test_framework.h"

#include "test_functions.c"

static void __print_test_header(struct test *t)
{
	printf("RUNNING TEST %s\n", t->name);
	printf("=============");
	
	{
		int i, len = strlen(t->name);
		for (i = 0; i < len; ++i)
			putchar('=');
	}
}

static void __print_test_result(struct test *t)
{
	putchar('\n');
	printf("RESULT: %s\n", t->func() == TEST_SUCCESS ? S(TEST_SUCCESS) : S(TEST_FAILURE));
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

	for (; t->func != NULL; ++t)
	{
		__print_test_header(t);
		__print_test_result(t);
		__print_test_footer();
	}
#endif /* RUN_TESTS */
}