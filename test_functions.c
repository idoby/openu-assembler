/*	ALL functions must be static.
	Functions beginning with __TEST_ are actual test functions. */
#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"
#include "assembler.h"

static void __print_symbols(table_element *element)
{
	symbol *sym = table_entry(element);

	if (sym->name != NULL)
		printf("SYMBOL: %s TYPE: %d\n", sym->name, sym->type);
}

static void __print_refs(instruction *inst)
{
	printf("REF: %s\n", inst->inst);
}

static int __TEST_symbol_table(void)
{
	assembler assem;
	symbol *moo = NULL;
	instruction inst1 = {"MOV"};
	instruction inst2 = {"BLABLA"};

	table_init(&assem.sym_table);

	table_new_symbol(&assem.sym_table, "MOO", EXTERN);
	table_new_symbol(&assem.sym_table, "MEEEEEEH", INTERN);
	table_new_symbol(&assem.sym_table, "ZZZZZZZZZZZ", INTERN_UNDEFINED);
	table_new_symbol(&assem.sym_table, "A symbol with more than 30 characters should get truncated", INTERN_UNDEFINED);

	test_assert(table_find_symbol(&assem.sym_table, "MOO") != NULL, "symbol 'MOO' not found.");
	test_assert(table_find_symbol(&assem.sym_table, "SHIT") == NULL, "non-existent symbol 'SHIT' found?");

	table_traverse(&assem.sym_table, __print_symbols);

	moo = table_find_symbol(&assem.sym_table, "MOO");
	table_add_reference(moo, &inst1);
	table_add_reference(moo, &inst2);

	table_consume_references(moo, __print_refs);

	table_destroy(&assem.sym_table);

	test_assert(assem.sym_table.root_node == NULL, "table not destroyed properly.");

	return TEST_SUCCESS;
}

static int __TEST_list_test(void)
{
	struct list_test {
		char name[20];
		list more;
	};

	struct list_test l1 = {"L1", {NULL, NULL}}, l2 = {"L2", {NULL, NULL}}, l3 = {"L3", {NULL, NULL}};
	struct list_test *lp, *safe;
	list l;
	unsigned int i = 0;

	list_init(&l);

	list_insert_before(&l, &l1.more);
	list_insert_before(&l, &l2.more);
	list_insert_before(&l, &l3.more);

	list_for_each_entry_safe(&l, lp, safe, struct list_test, more)
	{
		printf("%s\n", lp->name);
		++i;
	}

	test_assert(i == 3, "Not all list elements iterated!");

	return TEST_SUCCESS;
}

static int __TEST_list_test_empty(void)
{
	struct list_test {
		char name[20];
		list more;
	};

	struct list_test *lp, *safe;
	list l;
	unsigned int i = 0;
	struct list_test *l1 = malloc(sizeof(*l1));

	test_assert(l1 != NULL, "Can't malloc()!");

	list_init(&l);

	list_for_each_entry_safe(&l, lp, safe, struct list_test, more)
		++i;

	printf("Iterated %d times.\n", i);
	test_assert(i == 0, "Loop body entered!");

	i = 0;

	list_insert_after(&l, &l1->more);

	list_for_each_entry_safe(&l, lp, safe, struct list_test, more)
	{
		free(lp);
		++i;
	}

	printf("Iterated %d times.\n", i);
	test_assert(i == 1, "Loop body not entered once!");

	return TEST_SUCCESS;
}
/*	The list of tests to be run.
	Add new tests here. */
#define TEST_LIST(test_list_entry)	\
	test_list_entry(symbol_table)	\
	test_list_entry(list_test)		\
	test_list_entry(list_test_empty)
