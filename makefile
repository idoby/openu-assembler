SOURCES		= main.c intrusive_tree.c intrusive_list.c symbol_table.c
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

.PHONY: clean

clean:
	rm as