#include <stdio.h>
#include "utils.h"
#include "intrusive_list.h"
#include "symbol_table.h"
#include "assembler.h"
#include "tests.h"

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

int main()
{
	printf("sizeof scratch: %d. sizeof memcell: %d.\n", sizeof(struct scratch_space), sizeof(((struct scratch_space*)0)->cells[0]));
	run_tests();
	return 0;
}