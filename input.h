#ifndef __INPUT_H
#define __INPUT_H

#define MAX_BUF 1024
#define MAX_FILE_NAME 256

typedef struct input_context {
	char file_name[MAX_FILE_NAME];
	FILE* f;
	char line[MAX_BUF];
	unsigned int line_number;
} input_context;

typedef struct input_ops {
	input_context*	(*input_init)		(char* file_name);
	char*			(*input_get_line)	(input_context* in);
	void			(*input_destroy_line)(input_context* in);
	void			(*input_destroy)	(input_context* in);
} input_ops;

#endif