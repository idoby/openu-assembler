#include "error.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

error* error_make(unsigned int line, char *format, ...)
{
	error *err;
	va_list ap;

	if (format == NULL)
		return NULL;

	if ((err = malloc(sizeof(*err))) == NULL)
		return NULL;

	err->line = line;

	va_start(ap, format);
	vsprintf(err->text, format, ap);
	va_end(ap);

	list_init(&err->errors);

	return err;
}

void error_print(error *err)
{
	if (err == NULL)
		return;

	/* Error is associated with a line. */
	if (err->line != ERROR_NO_LINE)
		printf("Error on line #%d: ", err->line);
	
	printf("%s\n", err->text);
}

void error_destroy(error *err)
{
	if (err == NULL)
		return;

	list_remove(&err->errors);
	free(err);
}
