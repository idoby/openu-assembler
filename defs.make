SOURCES         = as.c assembler.c
SOURCE_DIRS		= data_structures modules/default
EXTRA_DEPS      = $(wildcard *.h modules/*.h)
TARGET			= as

ifeq ($(WITH_TESTS), y)
        FLAGS += -DRUN_TESTS
        SOURCE_DIRS += tests
endif

.PHONY: run_tests
run_tests: clean as valgrind_test

.PHONY: valgrind_test
valgrind_test:
	clear; \
	valgrind -v --track-origins=yes --leak-check=full --read-var-info=yes ./as $(TEST_FILE) 2> test_log_valgrind_errors; \
	grep 'ERROR SUMMARY' test_log_valgrind_errors;