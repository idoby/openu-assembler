#ifndef __TRANSLATE_H
#define __TRANSLATE_H

#include <data_structures/intrusive_list.h>
#include <data_structures/symbol_table.h>
#include <data_structures/scratch_space.h>

typedef void translate_context;

typedef enum translate_line_error {
	TRANSLATE_LINE_SYNTAX_ERROR,
	TRANSLATE_LINE_ALLOC_ERROR,
	TRANSLATE_LINE_BAD_PARAMS,
	TRANSLATE_LINE_SUCCESS
} translate_line_error;

typedef enum translate_error {
	TRANSLATE_SUCCESS,
	TRANSLATE_BAD_PARAMS,
	TRANSLATE_BAD_PROGRAM,
	TRANSLATE_CANT_RESOLVE
} translate_error;

typedef struct translate_ops {
						/* Module constructor. */
	translate_context*	(*init)
						(symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch);
						/* Module destructor. */
	void				(*destroy)(translate_context *tc);
						/* Translate a single line. */
	translate_line_error(*translate_line)(translate_context *tc, char *line);
						/* Is the program valid? Should we print errors or finalize? */
	unsigned int		(*is_program_valid)(translate_context* tc);
						/* Finalize translation. At this point the data structures are ready for output. */
	translate_error		(*finalize)(translate_context *tc);
} translate_ops;

#endif