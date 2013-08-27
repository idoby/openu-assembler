#ifndef __ASSEMBLER_H
#define __ASSEMBLER_H

/* Data structures we need for the assembler. */
#include <data_structures/intrusive_list.h>
#include <data_structures/scratch_space.h>
#include <data_structures/symbol_table.h>
#include <data_structures/error.h>

/* Operations struct definitions for each subsystem. */
#include <modules/input.h>
#include <modules/translate.h>
#include <modules/output.h>

/* Assembler data structure containing all the stuff
   we need for processing a single file. */
typedef struct assembler {
	input_ops           input_ops;      /* Input operations. */
	input_context       *ic;            /* Input context object for saving input module state. */

	translate_ops       translate_ops;  /* Translation operations. */
	translate_context   *tc;            /* Translate context object for saving translate module state. */

	output_ops          output_ops;     /* Output operations. */
	output_context      *oc;            /* Output context object for saving output module state. */

	scratch_space       i_scratch;      /* Scratch space for instructions. */
	scratch_space       d_scratch;      /* Scratch space for data. */
	symbol_table        sym_table;      /* Symbol table. */
	list                errors;         /* List of errors in the parsed file. */
} assembler;

/* Initialize an assembler data structure. */
void assembler_init(assembler *ass);

/* Destroy the assembler object. */
void assembler_destroy(assembler *ass);

/* Process a file. */
void assembler_process(assembler *ass, char *file_name);

#define assembler_dispatch(ass, module_name) \
	do { \
		ass.input_ops = module_name ## _input_ops; \
		ass.translate_ops = module_name ## _translate_ops; \
		ass.output_ops = module_name ## _output_ops; \
	} while(0)

#endif