#ifndef __DEFAULT_OUTPUT_H
#define __DEFAULT_OUTPUT_H

#include <modules/output.h>

output_context*	default_output_init
				(char* file_name, symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch, list *errors);
unsigned int	default_output_dump (output_context *oc);
void			default_output_destroy(output_context *oc);

extern output_ops default_output_ops;

#endif