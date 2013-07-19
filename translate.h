#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include "intrusive_list.h"
#include "symbol_table.h"
#include "scratch_space.h"

typedef void translate_context;

typedef enum translate_line_error {
	TRANSLATE_LINE_ERROR,
	TRANSLATE_LINE_SUCCESS
} translate_line_error;

typedef enum translate_error {
	TRANSLATE_SUCCESS,
	TRANSLATE_CANT_RESOLVE
} translate_error;

typedef struct translate_ops {
						/* Module constructor. */
	translate_context*	(*translate_init)
						(list *insts, symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch);
						/* Module destructor. */
	void				(*translate_destroy)(translate_context *tc);
						/* Translate a single line. */
	translate_line_error(*translate_line)(translate_context *tc, char *line);
						/* Finalize translation. At this point the data structures are ready for output. */
	translate_error		(*translate_finalize)(translate_context *tc);
} translate_ops;

#endif