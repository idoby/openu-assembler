#include <stdlib.h>
#include <stdio.h>

#include <assembler.h>
#include <modules/default/default_input.h>
#include <modules/default/default_translate.h>
#include <modules/default/default_output.h>

#include <tests/tests.h>

int main(int argc, char** argv)
{
	/* Run tests if they are enabled and we
	   don't have any parameters. */
	if (argc == 1)
		return (!(run_tests() == TEST_SUCCESS));

	/* Run over all arguments. */
	while (--argc > 0)
	{
		/* Assemble the assembler! :D */
		assembler ass;
		assembler_init(&ass);
		assembler_dispatch(ass, default);

		/* Process the file! */
		printf("Processing file %s:\n", argv[argc]);
		assembler_process(&ass, argv[argc]);

		/* Destroy the assembler object. */
		assembler_destroy(&ass);
	}

	return 0;
}