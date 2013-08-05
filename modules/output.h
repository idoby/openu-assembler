#ifndef __OUTPUT_H
#define __OUTPUT_H

#include <data_structures/symbol_table.h>
#include <data_structures/scratch_space.h>


#define MAX_FILE_NAME	256

typedef void output_context;

typedef struct output_ops {
	output_context*	(*init)							/* A time to build up. */
					(char* file_name, symbol_table *syms, scratch_space *d_scratch, scratch_space *i_scratch);
	unsigned int	(*dump)(output_context *oc);		/* To write. */
					
	void			(*destroy)(output_context *oc);	/* And to break down. */
} output_ops;

#endif