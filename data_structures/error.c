#include "error.h"

#include <string.h>
#include <stdlib.h>

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

void error_destroy(error *err)
{
	if (err == NULL)
		return;

	list_remove(&err->errors);
	free(err);
}
