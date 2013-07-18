#ifndef __DEFAULT_TRANSLATE_H
#define __DEFAULT_TRANSLATE_H

#include "translate.h"

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
} instruction;

/* Exported out of the module for testing, but still should not be used! */

/* Returns a new instruction object with the prototype already set, based on name. */
instruction*	default_instruction_make(char *name);
/* Destroys an instruction object given by make_instruction. */
void			default_instruction_destroy(instruction *inst);

#endif