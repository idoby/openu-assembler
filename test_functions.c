/*	ALL functions must be static.
	Functions beginning with __TEST_ are actual test functions. */
#include <stdlib.h>
#include <stdio.h>
#include "symbol_table.h"
#include "assembler.h"

#include "default_input.h"
#include "default_translate.h"

static int __TEST_default_input(void)
{
	input_ops input = default_input_ops;

	input_context *ic = input.input_init("ps");

	test_assert(ic != NULL, "Failed to init input with \"ps.as\"!");

	/* Read all lines. */
	while (input.input_get_line(ic) != NULL);

	printf("# of lines in test file: %d.\n", input.input_get_line_number(ic));

	input.input_destroy(ic);

	return TEST_SUCCESS;
}

static int __TEST_default_translate(void)
{
	translate_ops trans = default_translate_ops;
	translate_context *tc;
	list insts;
	symbol_table syms;
	scratch_space s1, s2;

	list_init(&insts);
	table_init(&syms);

	tc = trans.translate_init(&insts, &syms, &s1, &s2);

	/* Data tests. */
	test_assert(trans.translate_line(tc, "    SYM: .data 7\n") == TRANSLATE_LINE_SUCCESS,
				"Line NOT parsed successfully!");
	test_assert(trans.translate_line(tc, "    lab2: .data -76   , 8\n") == TRANSLATE_LINE_SUCCESS,
				"Line NOT parsed successfully!");
	test_assert(trans.translate_line(tc, " .data 12,78,-48\n") == TRANSLATE_LINE_SUCCESS,
				"Line NOT parsed successfully!");
	test_assert(trans.translate_line(tc, " .data \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data ") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, "     .data +7\n") == TRANSLATE_LINE_SUCCESS,
				"line NOT parsed successfully!");
	test_assert(trans.translate_line(tc, "           : .data 7\n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, "2lab:    .data \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data 8, \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data 8B \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data -8    B \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data 8B, \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data 45, +8B4 \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data ,45   \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");
	test_assert(trans.translate_line(tc, " .data 45  6 \n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");

	/* .string tests */
	test_assert(trans.translate_line(tc, "    SYM: .string \"hello, stupid world!\"\n") == TRANSLATE_LINE_SUCCESS,
				"Line NOT parsed successfully!");
	test_assert(trans.translate_line(tc, "    lab2: .string \"hi   , 8\n") != TRANSLATE_LINE_SUCCESS,
				"Line parsed successfully!");

	table_destroy(&syms);

	trans.translate_destroy(tc);

	return TEST_SUCCESS;
}

static void __print_inst(instruction *inst)
{
	if (inst == NULL)
		return;

	printf("instruction name: %s\n", 	inst->proto->name);
	printf("instruction opcode: %d\n", 	inst->proto->opcode);
	printf("instruction operands: %d\n",inst->proto->num_operands);
	printf("instruction src allowed: %d\n", inst->proto->allowed_modes[1]);
	printf("instruction dst allowed: %d\n", inst->proto->allowed_modes[0]);
}

static int __TEST_default_instruction_make(void)
{
	instruction *inst1, *inst2;

	/* Get an instruction that exists.*/
	inst1 = default_instruction_make("clr");

	/* Get one that doesn't. */
	inst2 = default_instruction_make("moo");

	test_assert(inst1 != NULL && strcmp(inst1->proto->name, "clr") == 0, "clr instruction not found!");
	test_assert(inst2 == NULL, "moo instruction exists?");

	__print_inst(inst1);

	default_instruction_destroy(inst1);

	return TEST_SUCCESS;
}

static void __print_symbols(table_element *element)
{
	symbol *sym = table_entry(element);

	if (sym->name != NULL)
		printf("SYMBOL: %s TYPE: %d\n", sym->name, sym->type);
}

struct dummy_ref {
	int an_int;
	char a_char;
} dref1, dref2;

static void __print_refs(void *ref)
{
	struct dummy_ref *dref = ref; /* Converting from void* implicitly is fine.*/
	printf("REF: %d, %c\n", dref->an_int, dref->a_char);
}

static int __TEST_symbol_table(void)
{
	struct dummy_ref dref1 = {1234, 'A'}, dref2 = {431, 'Z'};

	assembler assem;
	symbol *moo = NULL;

	table_init(&assem.sym_table);

	table_new_symbol(&assem.sym_table, "MOO", EXTERN);
	table_new_symbol(&assem.sym_table, "MEEEEEEH", INTERN);
	table_new_symbol(&assem.sym_table, "ZZZZZZZZZZZ", INTERN_UNDEFINED);
	table_new_symbol(&assem.sym_table, "A symbol with more than 30 characters should get truncated", INTERN_UNDEFINED);

	test_assert(table_find_symbol(&assem.sym_table, "MOO") != NULL, "symbol 'MOO' not found.");
	test_assert(table_find_symbol(&assem.sym_table, "SHIT") == NULL, "non-existent symbol 'SHIT' found?");

	table_traverse(&assem.sym_table, __print_symbols);

	moo = table_find_symbol(&assem.sym_table, "MOO");
	table_add_reference(moo, &dref1);
	table_add_reference(moo, &dref2);

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
#define TEST_LIST(list_entry)		\
	list_entry(default_input)		\
	list_entry(default_translate)	\
	list_entry(default_instruction_make)	\
	list_entry(symbol_table)		\
	list_entry(list_test)			\
	list_entry(list_test_empty)
