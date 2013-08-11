#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "default_output.h"

typedef struct default_output_context {
	char	object_file_name[MAX_FILE_NAME];
	FILE*	object_file;

	char	externs_file_name[MAX_FILE_NAME];
	FILE*	externs_file;

	char	entries_file_name[MAX_FILE_NAME];
	FILE*	entries_file;
	
	scratch_space 	*i_scratch;
	scratch_space 	*d_scratch;

	symbol_table 	*syms;

	list 			*errors;
} default_output_context; 

output_ops default_output_ops = {	default_output_init,
									default_output_dump,
									default_output_destroy};

output_context*	default_output_init
				(char* file_name, symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch, list *errors)
{
	default_output_context *doc;

	if (file_name == NULL || i_scratch == NULL || d_scratch == NULL || syms == NULL || errors == NULL)
		return NULL;

	if ((doc = malloc(sizeof(*doc))) == NULL)
		return NULL;

	/* TODO: initialize. */

	return doc;
}

unsigned int default_output_dump(output_context *oc)
{
	oc = oc;
	return 1;
	/* TODO: actually implement the module. */
}

void default_output_destroy(output_context *oc)
{
	default_output_context *doc = (default_output_context*)oc;

	if (doc == NULL)
		return;

	/* TODO: destroy. */

	free(doc);

}
