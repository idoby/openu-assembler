/*	ALL functions must be static.
	Functions beginning with __TEST_ are actual test functions. */

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

	table_new_symbol(&assem.sym_table, "CRAP", EXTERN);
	table_new_symbol(&assem.sym_table, "SHITFUCK", INTERN);

	test_assert(table_find_symbol(&assem.sym_table, "CRAP") != NULL, "symbol 'CRAP' not found.");
	test_assert(table_find_symbol(&assem.sym_table, "SHIT") == NULL, "non-existent symbol 'SHIT' found?");

	table_traverse(&assem.sym_table, __print_symbols);
	table_destroy(&assem.sym_table);

	return TEST_SUCCESS;
}

/* The list of tests to be run. */
#define TEST_LIST(test_list_entry) \
	test_list_entry(symbol_table)

struct test tests[] =
			{	TEST_LIST(TEST_STRUCT_ENTRY)
				{"NULL", NULL}			};