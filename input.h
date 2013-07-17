#ifndef __INPUT_H
#define __INPUT_H

#define MAX_BUF 1024
#define MAX_FILE_NAME 256

typedef void input_context;

typedef struct input_ops {
	input_context*	(*input_init)		(char* file_name);
	char*			(*input_get_line)	(input_context* ic);
	unsigned int 	(*input_get_line_number)(input_context* ic);
	void			(*input_destroy_line)(input_context* ic);
	void			(*input_destroy)	(input_context* ic);
} input_ops;

#endif