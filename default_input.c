#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "default_input.h"

input_ops default_input_ops =
		{default_input_init, default_input_get_line, default_input_destroy_line, default_input_destroy};

const char default_file_extension[] = ".as";
const size_t default_file_ext_len = sizeof(default_file_extension) - 1;

input_context* default_input_init(char *file_name)
{
	input_context* in;

	if (file_name == NULL)
		return NULL;

	/* Is the filename + extension short enough to fit in the buffer? */
	if (strlen(file_name) + default_file_ext_len >= MAX_FILE_NAME)
		return NULL;

	if ((in = malloc(sizeof(*in))) == NULL)
		return NULL;

	in->line_number = 0;

	/* Copy the file name and append the ".as" we need to open it. */
	strncpy(in->file_name, file_name, MAX_FILE_NAME);
	strncat(in->file_name, default_file_extension, default_file_ext_len);


	if ((in->f = fopen(in->file_name, "r")) == NULL)
	{
		free(in);
		return NULL;
	}

	return in;
}

char* default_input_get_line(input_context* in)
{
	if (in == NULL || in->f == NULL)
		return NULL;

	if (fgets(in->line, MAX_BUF, in->f) == NULL)
		return NULL;

	in->line_number++;
	
	return in->line;
}

void default_input_destroy_line(input_context* in)
{
	in = in; /* Nothing to do here, but stop the compiler from crying. */
}

void default_input_destroy(input_context* in)
{
	if (in == NULL)
		return;

	fclose(in->f);

	free(in);
}