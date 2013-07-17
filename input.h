#ifndef __INPUT_H
#define __INPUT_H

typedef void input_context;

typedef struct input_ops {
	input_context*	(*input_init)		(char* file_name);		/* Module constructor. */
	char*			(*input_get_line)	(input_context* ic);	/* Get a line from the file. */
	unsigned int 	(*input_get_line_number)(input_context* ic);	/* Get the current line number. */
	void			(*input_destroy_line)(input_context* ic);	/* Destroy the line object. */
	void			(*input_destroy)	(input_context* ic);	/* Module destructor. */
} input_ops;

#endif