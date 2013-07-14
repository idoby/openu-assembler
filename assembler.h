#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

#include "intrusive_list.h"
#include "scratch_space.h"
#include "symbol_table.h"

typedef struct assembler_ops {
	void*	(*input_init)		(char* file_name);
	char*	(*input_get_line)	(void* in, void);
	void	(*input_destroy_line)(void* in, char ch);
	void	(*input_destroy)	(void* in);
	void	(*output_init)		(char* file_name);
} assembler_ops;

typedef struct assembler {
	assembler_ops 	ops;			/* Assembler operations. */
	scratch_space 	i_scratch;		/* Scratch space for instructions. */
	scratch_space 	d_scratch;		/* Scratch space for data. */
	symbol_table 	sym_table;		/* Symbol table. */
	list			instructions;	/* List of parsed instructions. */
} assembler;

#endif