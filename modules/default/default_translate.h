#ifndef __DEFAULT_TRANSLATE_H
#define __DEFAULT_TRANSLATE_H

#include <modules/translate.h>

#define MAX_OPERANDS				2

typedef struct ins_prototype {
	char *name;
	unsigned int opcode;
	unsigned int num_operands;
	unsigned int allowed_modes[MAX_OPERANDS];
} ins_prototype;

/* A type representing one parsed instruction. */
typedef struct instruction {
	ins_prototype *proto;
	unsigned int type;
	unsigned int dbl;
	unsigned int comb;
	struct address *operands[MAX_OPERANDS];	/* First is destination, second is source. */
	list	insts;
	unsigned int src_line;					/* Source line number in the input file. */
} instruction;

translate_context*	default_translate_init
					(symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch);
void				default_translate_destroy(translate_context *tc);
translate_line_error default_translate_line(translate_context *tc, char *line);
unsigned int		default_is_program_valid(translate_context *tc);
translate_error		default_translate_finalize(translate_context *tc);

extern translate_ops default_translate_ops;

/* Exported out of the module for testing, but still should not be used! */

/* Returns a new instruction object with the prototype already set, based on name. */
instruction*	default_instruction_make(char *name);
/* Destroys an instruction object given by make_instruction. */
void			default_instruction_destroy(instruction *inst);

#endif