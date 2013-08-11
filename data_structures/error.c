#include "error.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

error* error_make(char *text, unsigned int line)
{
	error *err;

	if (text == NULL)
		return NULL;

	if ((err = malloc(sizeof(*err))) == NULL)
		return NULL;

	err->line = line;
	strncpy(err->text, text, ERROR_TEXT_MAX);
	list_init(&err->errors);

	return err;
}

void error_print(error *err)
{
	if (err == NULL)
		return;

	/* Error is associated with a line. */
	if (err->line != ERROR_NO_LINE)
		printf("@ line #%d: ", err->line);
	
	printf("%s\n", err->text);
}

void error_destroy(error *err)
{
	if (err == NULL)
		return;

	list_remove(&err->errors);
	free(err);
}
