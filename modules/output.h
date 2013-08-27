#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <data_structures/symbol_table.h>
#include <data_structures/scratch_space.h>
#include <data_structures/intrusive_list.h>

#define MAX_FILE_NAME 256

typedef void output_context;

typedef struct output_ops {
	/* Module constructor. */
	output_context* (*init)
	                (char* file_name,
	                symbol_table *syms,
	                scratch_space *d_scratch,
	                scratch_space *i_scratch,
	                list *errors);

	/* Output the program data to file(s). */
	unsigned int    (*dump) (output_context *oc);
	/* Module destructor. */
	void            (*destroy)(output_context *oc);
} output_ops;

#endif