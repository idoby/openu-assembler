#define TEST_NAME_LEN 30

#define S(s) #s
#define TEST_FUNC_NAME(test) __TEST_ ## test
#define TEST_FUNC_PROTOTYPE(test) static int TEST_FUNC_NAME(test)(void);
#define TEST_STRUCT_ENTRY(test) { #test, TEST_FUNC_NAME(test) },

typedef int (*test_func)(void);

enum test_error {
	TEST_SUCCESS,
	TEST_FAILURE
};

struct test {
	char name[TEST_NAME_LEN + 1];
	test_func func;
};