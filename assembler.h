#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

#include "scratch_space.h"
#include "symbol_table.h"

struct assembler_ops {
	int 	(*input_init)		(void);
	char 	(*input_getchar)	(void);
	void	(*input_ungetchar)	(char ch);
};

typedef struct assembler {
	struct assembler_ops ops;		/* Assembler operations. */
	struct scratch_space i_scratch;	/* Scratch space for instructions. */
	struct scratch_space d_scratch;	/* Scratch space for data. */
	symbol_table sym_table;			/* Symbol table. */
	/* TODO: add list of opcode lines. */
} assembler;

#endif