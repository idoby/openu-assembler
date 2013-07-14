#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include "intrusive_list.h"

#define MAX_OPERANDS				2

typedef struct ins_prototype {
	char *name;
	unsigned int opcode;
	unsigned int num_operands;
	unsigned int allowed_modes[MAX_OPERANDS];
} ins_prototype;

/* A type representing one parsed instruction. */
typedef struct instruction {
	struct ins_prototype *proto;
	unsigned int type;
	unsigned int dbl;
	unsigned int comb;
	void *operands[MAX_OPERANDS];	/* First is destination, second is source. */
	list	insts;
} instruction;

/* Returns a new instruction object with the prototype already set, based on name. */
/* Exported out of the module for testing. */
instruction* make_new_instruction(char *name);

#endif