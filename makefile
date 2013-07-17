SOURCES		= main.c intrusive_tree.c intrusive_list.c symbol_table.c default_input.c default_translate.c
FLAGS		= -ansi -pedantic -Wall -Wextra -g -o as
EXTRA_DEPS	= *.h
WITH_TESTS = y

ifeq ($(WITH_TESTS), y)
	FLAGS += -DRUN_TESTS
	SOURCES += tests.c
	EXTRA_DEPS += test_functions.c
endif

all: as

as: $(SOURCES) $(EXTRA_DEPS)
	gcc $(FLAGS) $(SOURCES)

.PHONY: clean run_tests

clean:
	rm as

run_tests: clean as valgrind_test

valgrind_test:
	clear; \
	valgrind -v --track-origins=yes --leak-check=full --read-var-info=yes ./as 2> test_log_valgrind_errors; \
	grep 'ERROR SUMMARY' test_log_valgrind_errors;