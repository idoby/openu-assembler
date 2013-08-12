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

#define __print_errors() \
	do {	\
		error *err;		\
		list_for_each_entry(&ass->errors, err, error, errors)	\
		error_print(err);									\
	} while(0)

void assembler_process(assembler *ass, char* file_name)
{
	unsigned int program_valid = 1;
	translate_error finalize_result;
	char* line = NULL;
	char real_file_name[MAX_FILE_NAME] = {0};

	if (ass == NULL || file_name == NULL)
		return;

	/* Initialize modules. */
	if ((ass->ic = ass->input_ops.init(file_name, &ass->errors)) == NULL)
	{
		printf("For argument %s:\n", file_name);
		__print_errors();
		return;
	}

	ass->input_ops.get_real_file_name(ass->ic, real_file_name);

	if ((ass->tc = ass->translate_ops.init(	&ass->sym_table,
											&ass->i_scratch,
											&ass->d_scratch,
											&ass->errors)) == NULL)
	{
		printf("In file %s:\n", real_file_name);
		__print_errors();
		goto assembler_process_exit;
	}

	if ((ass->oc = ass->output_ops.init(file_name,
										&ass->sym_table,
										&ass->i_scratch,
										&ass->d_scratch,
										&ass->errors)) == NULL)
	{
		printf("In file %s:\n", real_file_name);
		__print_errors();
		goto assembler_process_exit;
	}

	/* Process the lines from the input file one by one. */
	while ((line = ass->input_ops.get_line(ass->ic)) != NULL)
	{
		ass->translate_ops.translate_line(ass->tc, line);
		ass->input_ops.destroy_line(ass->ic);
	}

	program_valid 	= ass->translate_ops.is_program_valid(ass->tc);
	finalize_result = ass->translate_ops.finalize(ass->tc);

	if (!program_valid || finalize_result != TRANSLATE_SUCCESS || !ass->output_ops.dump(ass->oc))
	{
		printf("In file %s:\n", real_file_name);
		__print_errors();
	}

assembler_process_exit:
	/* Destroy the modules. */
	ass->input_ops.destroy(ass->ic);
	ass->ic = NULL;
	
	ass->translate_ops.destroy(ass->tc);
	ass->tc = NULL;

	ass->output_ops.destroy(ass->oc);
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
	list_for_each_entry_safe(	&ass->errors,
								err,
								safe,
								error,
								errors)
		error_destroy(err);
}