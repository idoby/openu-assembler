#ifndef __INPUT_H
#define __INPUT_H

#include <data_structures/intrusive_list.h>

#define MAX_FILE_NAME 256

typedef void input_context;

typedef struct input_ops {
	input_context*	(*init)		(char* file_name, list *errors);		/* Module constructor. */
	void			(*get_real_file_name)(input_context *ic, char file_name[MAX_FILE_NAME]);
	char*			(*get_line)	(input_context* ic);	/* Get a line from the file. */
	unsigned int 	(*get_line_number)(input_context* ic);	/* Get the current line number. */
	void			(*destroy_line)(input_context* ic);	/* Destroy the line object. */
	void			(*destroy)	(input_context* ic);	/* Module destructor. */
} input_ops;

#endif