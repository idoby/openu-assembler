#include <stdio.h>
#include <assert.h>
#include "utils.h"
#include "intrusive_list.h"
#include "symbol_table.h"
#include "assembler.h"

#define MAX_BUF 1000

typedef enum {
	label,
	directive_entry,
	directive_extern,
	directive_data,
	directive_string,
	opcode,
	opcode_spec,
	opcode_two_args,
	opcode_one_arg,
	opcode_no_args,
	immediate_address,
	direct_address,
	register_address,
	relative_address,
	relative_offset,
	relative_label,
	sharp,
	number,
	curly_brace_open,
	curly_brace_close,
	asterisk

} token_type;

struct lexer_state;

typedef struct lexer_state (*state_func_p)(void);

struct lexer_state
{
	state_func_p 	next_state;				/* The next state to actually transition into. NULL if syntax is bad and we can't continue. */
	state_func_p 	expected_next_state; 	/* The expected next state in case we need to deal with a syntax error. */
};

void print_symbols(table_element *element)
{
	symbol *sym = table_entry(element);

	if (sym->name != NULL)
		printf("%s\n", sym->name);
}

int main()
{
	assembler assem;

	table_init(&assem.sym_table);
	table_new_symbol(&assem.sym_table, "CRAP", EXTERN);
	table_new_symbol(&assem.sym_table, "SHITFUCK", INTERN);
	assert(table_find_symbol(&assem.sym_table, "CRAP") != NULL);
	assert(table_find_symbol(&assem.sym_table, "SHIT") == NULL);
	table_traverse(&assem.sym_table, print_symbols);
	table_destroy(&assem.sym_table);

	return 0;
}