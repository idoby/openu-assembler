#include <stdio.h>
#include <assert.h>
#include "intrusive_list.h"
#include "symbol_table.h"

#define MAX_BUF 1000
#define MAX_MEM 2000

struct assembler_ops {
	int 	(*input_init)		(void);
	char 	(*input_getchar)	(void);
	void	(*input_ungetchar)	(char ch);
};

struct scratch_space {
	unsigned int buffer[MAX_MEM];
	unsigned int counter;
};

struct assembler {
	struct assembler_ops ops;		/* Assembler operations. */
	struct scratch_space iscratch;	/* Scratch space for instructions. */
	struct scratch_space dscratch;	/* Scratch space for data. */
};

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

int main()
{
	symbol_table table;

	table_init(&table);
	table_new_symbol(&table, "CRAP", EXTERN);
	assert(table_find_symbol(&table, "CRAP") != NULL);
	assert(table_find_symbol(&table, "SHIT") == NULL);
	table_destroy(&table);

	/*table_new_symbol(&table, "crap", EXTERN);*/
	return 0;
}