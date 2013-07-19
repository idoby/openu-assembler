#include "tests.h"

#define TEST_NAME_LEN 30

#define S(s) #s
#define S_(s) S(s)
#define S__LINE__ S_(__LINE__)
#define TEST_FUNC_NAME(test) __TEST_ ## test
#define TEST_FUNC_PROTOTYPE(test) static int TEST_FUNC_NAME(test)(void);
#define TEST_STRUCT_ENTRY(test) { #test, TEST_FUNC_NAME(test) },

#define FORMAT_ESCAPE "\033"
#define MAKE_FORMAT(f,s) FORMAT_ESCAPE f s FORMAT_RESET
#define FORMAT_RESET FORMAT_ESCAPE "[0m"
#define FORMAT_BOLD(s) MAKE_FORMAT("[1m",s)
#define FORMAT_UNDERLINE(s) MAKE_FORMAT("[4m",s)
#define FORMAT_RED(s) MAKE_FORMAT("[31m",s)
#define FORMAT_GREEN(s) MAKE_FORMAT("[32m",s)
#define FORMAT_YELLOW(s) MAKE_FORMAT("[33m",s)
#define FORMAT_BLUE(s) MAKE_FORMAT("[34m",s)

#define test_assert(cond, message) \
	do { \
		if (!(cond)) \
		{ \
			printf(FORMAT_RED("TEST FAILED:") " %s\n", message); \
			return TEST_FAILURE; \
		} \
	} while(0)

typedef int (*test_func)(void);

struct test {
	char name[TEST_NAME_LEN + 1];
	test_func func;
};