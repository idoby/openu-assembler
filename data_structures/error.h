#ifndef __ERROR_H
#define __ERROR_H

#include "intrusive_list.h"

#define ERROR_TEXT_MAX 512
#define ERROR_NO_LINE  0

typedef struct error {
	char text[ERROR_TEXT_MAX]; /* The error text to be printed. */
	unsigned int line;         /* The line number in the input file. */
	list errors;               /* The rest of the error list. */
} error;

/* Create a new error object using printf. */
error* error_make(unsigned int line, char *format, ...);
/* Print an error object. */
void error_print(error *err);
/* Destroy an error object created by error_make. */
void error_destroy(error *err);

#endif