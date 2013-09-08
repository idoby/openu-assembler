#include <stdio.h>
#include <assembler.h>

void assembler_init(assembler *ass)
{
	if (ass == NULL)
		return;

	scratch_init(&ass->i_scratch);
	scratch_init(&ass->d_scratch);

	list_init(&ass->errors);
	table_init(&ass->sym_table);

	/* So we can safely call free if something goes wrong. */
	ass->ic = NULL;
	ass->tc = NULL;
	ass->oc = NULL;
}

static void __print_errors(list *errors)
{
	error *err;
	list_for_each_entry(errors, err, error, errors)
	{
		putchar('\t');
		error_print(err);
	}
}

void assembler_process(assembler *ass, char *file_name)
{
	unsigned int program_valid = 0;
	char* line = NULL;
	char real_file_name[MAX_FILE_NAME] = {0};

	/* Operation structs to make access to modules shorter. */
	input_ops     iops;
	translate_ops tops;
	output_ops    oops;

	/* File name to output if we have errors. */
	char *error_file_name = file_name;

	if (ass == NULL || file_name == NULL)
		return;

	iops = ass->input_ops;
	tops = ass->translate_ops;
	oops = ass->output_ops;

	/* Initialize modules. */
	ass->ic = iops.init(file_name, &ass->errors);
	ass->tc = tops.init(&ass->sym_table, &ass->i_scratch,
	                    &ass->d_scratch, &ass->errors);
	ass->oc = oops.init(file_name, &ass->sym_table, &ass->i_scratch,
	                    &ass->d_scratch, &ass->errors);

	if (ass->ic == NULL || ass->tc == NULL || ass->oc == NULL)
		goto assembler_process_exit;

	/* Get the actual file name as defined by the input module.
	   At this point we want to use this name for error reporting too. */
	ass->input_ops.get_real_file_name(ass->ic, real_file_name);
	error_file_name = real_file_name;

	/* Process the lines from the input file one by one. */
	while ((line = iops.get_line(ass->ic)) != NULL)
	{
		tops.translate_line(ass->tc, line);
		iops.destroy_line(ass->ic);
	}

	program_valid = tops.is_program_valid(ass->tc) &&
	                tops.finalize(ass->tc) == TRANSLATE_SUCCESS;

	/* If the program has been translated successfully,
	   we should output it to the appropriate files. */
	if (program_valid)
		oops.dump(ass->oc);

assembler_process_exit:

	/* Print errors if there are any. */
	if (!list_empty(&ass->errors))
	{
		printf("In file %s:\n", error_file_name);
		__print_errors(&ass->errors);
	}

	/* Destroy the modules. */
	iops.destroy(ass->ic);
	ass->ic = NULL;
	
	tops.destroy(ass->tc);
	ass->tc = NULL;

	oops.destroy(ass->oc);
	ass->oc = NULL;
}

void assembler_destroy(assembler *ass)
{
	error *err, *safe;

	if (ass == NULL)
		return;

	ass->input_ops.destroy(ass->ic);
	ass->ic = NULL;
	
	ass->translate_ops.destroy(ass->tc);
	ass->tc = NULL;

	ass->output_ops.destroy(ass->oc);
	ass->oc = NULL;

	table_destroy(&ass->sym_table);

	/* Delete errors, if any exist. */
	list_for_each_entry_safe(&ass->errors, err, safe, error, errors)
		error_destroy(err);
}