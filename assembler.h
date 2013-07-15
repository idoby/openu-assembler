#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

/* Data structures we need for the assembler. */
#include "intrusive_list.h"
#include "scratch_space.h"
#include "symbol_table.h"

/* Operations struct definitions for each subsystem. */
#include "input.h"
#include "translate.h"
#include "output.h"

typedef struct assembler {
	input_ops	 	input_ops;		/* Input operations. */
	translate_ops	translate_ops;	/* Translation operations. */
	output_ops		output_ops;		/* Output operations. */
	scratch_space 	i_scratch;		/* Scratch space for instructions. */
	scratch_space 	d_scratch;		/* Scratch space for data. */
	symbol_table 	sym_table;		/* Symbol table. */
	list			instructions;	/* List of parsed instructions. */
} assembler;

#endif