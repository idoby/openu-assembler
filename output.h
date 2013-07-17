#ifndef __OUTPUT_H
#define __OUTPUT_H

typedef void output_context;

typedef struct output_ops {
	/* TODO: define this fully. */
	output_context*	(*output_init)							/* A time to build up. */
					(char* file_name, int out_object, int out_entries, int out_externs);
					
	void			(*output_destroy)(output_context* oc);	/* A time to break down. */
} output_ops;

#endif