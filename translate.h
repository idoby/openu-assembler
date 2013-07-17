#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include "intrusive_list.h"

#define MAX_OPERANDS				2

typedef struct translate_context {
	int temp; /* TODO: define this fully. */
} translate_context;

typedef struct translate_ops {
	int temp_member; /* TODO: define this part. */
} translate_ops;

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

enum addressing_mode {
	IMMEDIATE 	= 1,
	DIRECT		= 2,
	INDEX		= 4,
	REGISTER	= 8,
	ALL = IMMEDIATE | DIRECT | INDEX | REGISTER,
	NO_IMMEDIATE = DIRECT | INDEX | REGISTER,
	NONE = 0,
	ADDRESSING_MAX_MODE = REGISTER
};

/* Exported out of the module for testing: */

/* Returns a new instruction object with the prototype already set, based on name. */
instruction* instruction_make(char *name);
/* Destroys an instruction object given by make_instruction. */
void instruction_destroy(instruction *inst);

#endif