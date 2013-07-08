SOURCES = main.c intrusive_tree.c intrusive_list.c symbol_table.c

all: as

as: $(SOURCES)
	gcc -ansi -pedantic -Wall -Wextra -g -o as $(SOURCES)