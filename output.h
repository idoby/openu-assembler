#ifndef __OUTPUT_H
#define __OUTPUT_H

typedef struct output_ops {
		/* TODO: define this fully. */
		struct output_context*	(*output_init)		(char* file_name);
} output_ops;

#endif