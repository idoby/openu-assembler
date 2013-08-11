#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "default_input.h"

#include "assembler.h"

#define MAX_BUF 1024

typedef struct default_input_context {
	char			file_name[MAX_FILE_NAME];
	FILE*			f;
	char			line[MAX_BUF];
	unsigned int 	line_number;
	list*			errors;
} default_input_context;

input_ops default_input_ops =
		{	default_input_init,
			default_input_get_real_file_name,
			default_input_get_line,
			default_input_get_line_number,
			default_input_destroy_line,
			default_input_destroy};

static const char default_file_extension[] = ".as";
static const size_t default_file_ext_len = sizeof(default_file_extension) - 1;

input_context* default_input_init(char *file_name, list *errors)
{
	default_input_context* dic;

	if (file_name == NULL || errors == NULL)
		return NULL;

	/* Is the filename + extension short enough to fit in the buffer? */
	if (strlen(file_name) + default_file_ext_len >= MAX_FILE_NAME)
		return NULL;

	if ((dic = malloc(sizeof(*dic))) == NULL)
		return NULL;

	dic->line_number = 0;
	dic->errors = errors;

	/* Copy the file name and append the ".as" we need to open it. */
	strncpy(dic->file_name, file_name, MAX_FILE_NAME);
	strncat(dic->file_name, default_file_extension, default_file_ext_len);


	if ((dic->f = fopen(dic->file_name, "r")) == NULL)
	{
		error *err = error_make(ERROR_NO_LINE, "Unable to open file %s", dic->file_name);
		list_insert_before(errors, &err->errors);

		free(dic);
		return NULL;
	}

	return (input_context*)dic;
}

void default_input_get_real_file_name(input_context *ic, char file_name[MAX_FILE_NAME])
{
	default_input_context* dic = ic;

	if (dic == NULL || file_name == NULL)
		return;

	strncpy(file_name, dic->file_name, MAX_FILE_NAME);
}

char* default_input_get_line(input_context *ic)
{
	default_input_context *dic = ic;

	if (dic == NULL || dic->f == NULL)
		return NULL;

	if (fgets(dic->line, MAX_BUF, dic->f) == NULL)
		return NULL;

	dic->line_number++;
	
	return dic->line;
}

unsigned int default_input_get_line_number(input_context *ic)
{
	default_input_context *dic = ic;

	if (dic == NULL)
		return 0;

	return dic->line_number;
}

void default_input_destroy_line(input_context *ic)
{
	ic = ic; /* Nothing to do here, but stop the compiler from crying. */
}

void default_input_destroy(input_context *ic)
{
	default_input_context *dic = ic;

	if (dic == NULL)
		return;

	fclose(dic->f);

	free(dic);
}