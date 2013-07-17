#ifndef __TESTS_H
#define __TESTS_H

typedef enum test_error {
	TEST_SUCCESS,
	TEST_FAILURE
} test_error;

#ifdef RUN_TESTS
test_error run_tests(void);
#else
#define run_tests() TEST_SUCCESS
#endif

#endif