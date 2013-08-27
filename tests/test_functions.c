/* ALL functions must be static.
   Functions beginning with __TEST_ are actual test functions. */
#include <stdlib.h>
#include <stdio.h>
#include <utils.h>
#include <data_structures/symbol_table.h>
#include <data_structures/error.h>
#include <assembler.h>

#include <modules/default/default_input.h>
#include <modules/default/default_translate.h>
#include <modules/default/default_output.h>
#include <modules/default/default_instruction.h>

static int __TEST_default_input(void)
{
	input_ops input = default_input_ops;
	list errors;
	input_context *ic = NULL;

	list_init(&errors);

	ic = input.init("ps", &errors);

	test_assert(ic != NULL, "Failed to init input with \"ps.as\"!");

	/* Read all lines. */
	while (input.get_line(ic) != NULL);

	printf("# of lines in test file: %d.\n", input.get_line_number(ic));

	input.destroy(ic);

	return TEST_SUCCESS;
}

static int __default_translate_test_line(const char *line, int should_pass, const char *message)
{
	assembler ass;

	assembler_init(&ass);
	assembler_dispatch(ass, default);
	ass.tc = ass.translate_ops.init(&ass.sym_table, &ass.i_scratch, &ass.d_scratch, &ass.errors);

	test_assert((ass.translate_ops.translate_line(ass.tc, line) == TRANSLATE_LINE_SUCCESS) == should_pass,
				message);

	assembler_destroy(&ass);

	return TEST_SUCCESS;
}

static int __TEST_default_translate(void)
{
#define test_assert_parsed(line) \
	do { if (__default_translate_test_line((line), 1, "Line NOT parsed successfully: " S__LINE__)) return TEST_FAILURE; } while(0)

#define test_assert_not_parsed(line) \
	do { if (__default_translate_test_line((line), 0, "Line parsed successfully: " S__LINE__)) return TEST_FAILURE; } while(0)

	/* Comment tests. */
	test_assert_parsed("       ; this is a dandy comment\n");
	test_assert_parsed("       .entry a_label_appears ; this is a comment\n");
	test_assert_not_parsed("       .extern a_label_appe, ;comment\n");

	/* .data tests. */
	test_assert_parsed("    SYM: .data 7 ; with a comment\n");
	test_assert_parsed("    lab2: .data -76   , 8\n");
	test_assert_parsed(" .data 12,78,-48\n");
	test_assert_parsed("     .data +7\n");
	test_assert_parsed("	.data 7, -2568"); /* No \n */

	test_assert_not_parsed("	.data");
	test_assert_not_parsed("	.data 7, \n");
	test_assert_not_parsed("	.data 7, ;\n");
	test_assert_not_parsed(" .data \n");
	test_assert_not_parsed(" .data ");
	test_assert_not_parsed(" .da;ta ");
	test_assert_not_parsed("           : .data 7\n");
	test_assert_not_parsed("2lab:    .data \n");
	test_assert_not_parsed(" .data 8, \n");
	test_assert_not_parsed(" .data 8B \n");
	test_assert_not_parsed(" .data -8    B \n");
	test_assert_not_parsed(" .data 8B, \n");
	test_assert_not_parsed(" .data 45, +8B4 \n");
	test_assert_not_parsed(" .data ,45   \n");
	test_assert_not_parsed(" .data 45  6 \n");

	/* .string tests */
	test_assert_parsed("    SYM: .string \"hello, stupid world!\"\n");
	test_assert_parsed("    SYM: .string \"hello, stupid; world!\", \"fuck this shit, dawg\"	 ; AND A COMMENT\n");
	test_assert_parsed("    SYM: .string \"these	are 	tab 	separated!\", \"mother, 	fucker!\"\n");

	test_assert_not_parsed("   . string \"hello\"");
	test_assert_not_parsed("   .string \"hello\" , ");
	test_assert_not_parsed("    lab2: .string \"hi   , 8\n");

	/* .extern tests */
	test_assert_parsed("       .extern label\n");
	test_assert_parsed("    SYM: .extern word, s2h3i4t, a5sFRs");
	test_assert_parsed(" 	 .extern word, s2h3_i4t, a5sFRs\n");
	test_assert_parsed("r9:  .extern r8 ; should not pass because no :\n");


	test_assert_not_parsed("stop: 	 .extern word, s2h3_i4t, a5sFRs\n");
	test_assert_not_parsed("cmp: 	 .extern word, s2h3_i4t, a5sFRs\n");
	test_assert_not_parsed("r5: 	 .extern word, s2h3_i4t, a5sFRs\n");
	test_assert_not_parsed("r9  .extern r8 ; should not pass because no :\n");
	test_assert_not_parsed(".extern 2invalid_label");
	test_assert_not_parsed("  .extern r2 ; should not parse because r2 is a register\n");
	test_assert_not_parsed("  .extern r2 ");
	test_assert_not_parsed("r5:  .extern valid\n");

	/* .entry tests */
	test_assert_parsed(".entry some_label\n");
	test_assert_parsed("    SYM: .entry word, s2h3i---4t, a5sFRs");
	test_assert_parsed(".entry word, s2h3_i4t, a5sFRs\n");

	test_assert_not_parsed(".entry 2nvldlbl\n");
	test_assert_not_parsed(".entry stop");
	test_assert_not_parsed(".entry awesome,stop");

	/* Instruction tests. */
	test_assert_parsed("MAIN:	lea/0,0 STR{*LEN}, STRADD");
	test_assert_parsed("MAIN:	lea / 0 	, 0   STR{*LEN}, STRADD");
	test_assert_parsed("MAIN:	cmp /1 / 0 / 1 	, 0   STR{*LEN}, STRADD");
	test_assert_parsed("   cmp /1 / 0 / 1 	, 1   STRADD, STRADD");
	test_assert_parsed("MAIN:	cmp /1 / 0 / 1 	, 1   STR{*LEN}, #-876");
	test_assert_parsed("MAIN:	cmp /1 / 0 / 1 	, 1   STR{*LEN}, # +876");

	test_assert_not_parsed("MAIN:	lea/,0 STR{*LEN}, STRADD");
	test_assert_not_parsed("MAIN:	lea/0,0 STR{*LEN}, #-17");
	test_assert_not_parsed("MAIN:	lea / 0	,   STR{*LEN}, STRADD");
	test_assert_not_parsed("MAIN:	lea/0,0 STR{*LEN},#346"); /*Should not get parsed because of immediate operand. */
	test_assert_not_parsed("MAIN:	cmp /1 / 0  1 	, 0   STR{*LEN}, STRADD");
	test_assert_not_parsed("MAIN:	cmp /1 /0 , 1 	, 1   STR{*LEN}, STRADD");
	test_assert_not_parsed("MAIN:	cmp /0 /0/1 , 1 	, 1   STR{*LEN}, STRADD");
	test_assert_not_parsed("MAIN:	cmp    STR{*LEN}, STRADD");
	test_assert_not_parsed("MAIN:	cmp 0 /0/1 , 1 	, 1   STR{*LEN}, STRADD");

	return TEST_SUCCESS;
}

static void __print_inst(default_instruction *inst)
{
	if (inst == NULL)
		return;

	printf("instruction name: %s\n", inst->proto->name);
	printf("instruction opcode: %d\n", inst->proto->opcode);
	printf("instruction operands: %d\n", inst->proto->num_operands);
	printf("instruction src allowed: %d\n", inst->proto->allowed_modes[1]);
	printf("instruction dst allowed: %d\n", inst->proto->allowed_modes[0]);
}

static int __TEST_default_instruction_make(void)
{
	default_instruction *inst1, *inst2;

	/* Get an instruction that exists.*/
	inst1 = default_instruction_make("clr");

	/* Get one that doesn't. */
	inst2 = default_instruction_make("moo");

	test_assert(inst1 != NULL && strcmp(inst1->proto->name, "clr") == 0,
		    "clr instruction not found!");
	test_assert(inst2 == NULL, "moo instruction exists?");

	__print_inst(inst1);

	default_instruction_destroy(inst1);

	return TEST_SUCCESS;
}

static int __print_symbols(table_element *element, void *arg)
{
	symbol *sym = table_entry(element);

	if (sym->name != NULL)
		printf("SYMBOL: %s TYPE: %d\n", sym->name, sym->type);

	return 1;

	UNUSED_PARAM(arg); /* Shut up compiler. */
}

static void __print_refs(symbol *sym, unsigned int offset, void *arg)
{
	printf("REF: %s, %d\n", sym->name, offset);

	UNUSED_PARAM(arg); /* Shut up compiler. */
}

static int __TEST_symbol_table(void)
{
	assembler assem;
	symbol *moo = NULL;

	table_init(&assem.sym_table);

	table_new_symbol(&assem.sym_table, "MOO");
	table_new_symbol(&assem.sym_table, "MEEEEEEH");
	table_new_symbol(&assem.sym_table, "ZZZZZZZZZZZ");
	table_new_symbol(&assem.sym_table, "A symbol with more than 30 characters should get truncated");

	test_assert(table_find_symbol(&assem.sym_table, "MOO") != NULL,
		                      "symbol 'MOO' not found.");
	test_assert(table_find_symbol(&assem.sym_table, "DERP") == NULL,
		                      "non-existent symbol 'DERP' found?");

	table_traverse(&assem.sym_table, __print_symbols, NULL);

	moo = table_find_symbol(&assem.sym_table, "MOO");
	table_add_reference(moo, 1234);
	table_add_reference(moo, 431);

	table_consume_references(moo, __print_refs, NULL);

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

static int __TEST_error(void)
{
	error *err1, *err2;
	int i = 0;
	for (; i < 10; ++i)
	{
		err1 = error_make(456 + i, "TEST ERROR #%d", i);
		error_print(err1);
		error_destroy(err1);
	}

	err2 = error_make(ERROR_NO_LINE, "Overall number of errors: %d", i);
	error_print(err2);
	error_destroy(err2);

	return TEST_SUCCESS;
}

/*	The list of tests to be run.
	Add new tests here. */
#define TEST_LIST(list_entry) \
	list_entry(default_input) \
	list_entry(default_translate) \
	list_entry(default_instruction_make) \
	list_entry(symbol_table) \
	list_entry(error) \
	list_entry(list_test) \
	list_entry(list_test_empty)
