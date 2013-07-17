#include <stdlib.h>
#include <stdio.h>

#include "assembler.h"
#include "default_input.h"

#include "tests.h"

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
		ass.input_ops = default_input_ops;

		printf("%d\n", argc);
		printf("%s\n", argv[argc]);
	}



	return 0;
}