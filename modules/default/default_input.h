#ifndef __DEFAULT_INPUT_H
#define __DEFAULT_INPUT_H

#include <modules/input.h>

input_context* default_input_init(char *file_name, list *errors);
void           default_input_get_real_file_name(input_context *ic,
                                                char file_name[MAX_FILE_NAME]);
void           default_input_destroy(input_context* in);
char*          default_input_get_line(input_context* in);
unsigned int   default_input_get_line_number(input_context* in);
void           default_input_destroy_line(input_context* in);

extern input_ops default_input_ops;

#endif