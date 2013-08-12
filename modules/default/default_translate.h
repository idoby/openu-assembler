#ifndef __DEFAULT_TRANSLATE_H
#define __DEFAULT_TRANSLATE_H

#include <modules/translate.h>

translate_context*	default_translate_init
					(symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch, list *errors);
void				default_translate_destroy(translate_context *tc);
translate_line_error default_translate_line(translate_context *tc, const char *line);
unsigned int		default_is_program_valid(translate_context *tc);
translate_error		default_translate_finalize(translate_context *tc);

extern translate_ops default_translate_ops;

#endif