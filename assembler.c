#include <stdlib.h>
#include "assembler.h"

void assembler_init(assembler *ass)
{
	if (ass == NULL)
		return;

	/* TODO: init scratch spaces. */

	list_init(&ass->errors);
	table_init(&ass->sym_table);

	/* So we can safely call free if something goes wrong. */
	ass->ic = NULL;
	ass->tc = NULL;
	ass->oc = NULL;
}

void assembler_destroy(assembler *ass)
{
	error *err, *safe;
	/* TODO: destroy scratch spaces. */

	table_destroy(&ass->sym_table);

	/* Delete errors, if any exist. */
	list_for_each_entry_safe(	&ass->errors,
								err,
								safe,
								error,
								errors)
		error_destroy(err);
}