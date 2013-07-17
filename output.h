#ifndef __OUTPUT_H
#define __OUTPUT_H

typedef struct output_context {
	unsigned int temp; /* TODO: define this fully. */
} output_context;

typedef struct output_ops {
	/* TODO: define this fully. */
	output_context*	(*output_init)	(char* file_name, int out_object, int out_entries, int out_externs);
} output_ops;

#endif