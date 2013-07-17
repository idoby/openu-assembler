#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include "intrusive_list.h"
#include "symbol_table.h"
#include "scratch_space.h"

#define MAX_OPERANDS				2

typedef void translate_context;

typedef enum translate_error {
	TRANSLATE_LINE_ERROR,
	TRANSLATE_LINE_SUCCESS
} translate_error;

typedef struct translate_ops {
	translate_context*	(*translate_init)								/* Module constructor. */
						(list *insts, symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch);
	void				(*translate_destroy)(translate_context *tc);	/* Module destructor. */
	translate_error		(*translate_line)(translate_context *tc);		/* Translate a single line. */
} translate_ops;

#endif