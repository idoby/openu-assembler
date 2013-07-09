/*	ALL functions must be static.
	Functions beginning with __TEST_ are actual test functions. */
#include "symbol_table.h"
#include "assembler.h"

static void __print_symbols(table_element *element)
{
	symbol *sym = table_entry(element);

	if (sym->name != NULL)
		printf("SYMBOL: %s TYPE: %d\n", sym->name, sym->type);
}

static int __TEST_symbol_table(void)
{
	assembler assem;

	table_init(&assem.sym_table);

	table_new_symbol(&assem.sym_table, "MOO", EXTERN);
	table_new_symbol(&assem.sym_table, "MEEEEEEH", INTERN);
	table_new_symbol(&assem.sym_table, "ZZZZZZZZZZZ", INTERN_UNDEFINED);
	table_new_symbol(&assem.sym_table, "A symbol with more than 30 characters should get truncated", INTERN_UNDEFINED);

	test_assert(table_find_symbol(&assem.sym_table, "MOO") != NULL, "symbol 'MOO' not found.");
	test_assert(table_find_symbol(&assem.sym_table, "SHIT") == NULL, "non-existent symbol 'SHIT' found?");

	table_traverse(&assem.sym_table, __print_symbols);
	table_destroy(&assem.sym_table);

	test_assert(assem.sym_table.root_node == NULL, "table not destroyed properly.");

	return TEST_SUCCESS;
}

/*	The list of tests to be run.
	Add new tests here. */
#define TEST_LIST(test_list_entry) \
	test_list_entry(symbol_table)
