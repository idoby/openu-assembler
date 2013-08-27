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
	do { \
		error *err; \
		list_for_each_entry(&ass->errors, err, error, errors) \
		error_print(err); \
	} while(0)

void assembler_process(assembler *ass, char *file_name)
{
	unsigned int program_valid = 1;
	translate_error finalize_result;
	char* line = NULL;
	char real_file_name[MAX_FILE_NAME] = {0};
	input_ops     iops;
	translate_ops tops;
	output_ops    oops;

	if (ass == NULL || file_name == NULL)
		return;

	iops = ass->input_ops;
	tops = ass->translate_ops;
	oops = ass->output_ops;

	/* Initialize modules. */
	if ((ass->ic = iops.init(file_name, &ass->errors)) == NULL)
	{
		printf("For argument %s:\n", file_name);
		__print_errors();
		return;
	}

	ass->input_ops.get_real_file_name(ass->ic, real_file_name);

	if ((ass->tc = tops.init(&ass->sym_table,
	                         &ass->i_scratch,
	                         &ass->d_scratch,
	                         &ass->errors)) == NULL)
	{
		printf("In file %s:\n", real_file_name);
		__print_errors();
		goto assembler_process_exit;
	}

	if ((ass->oc = oops.init(file_name,
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
	while ((line = iops.get_line(ass->ic)) != NULL)
	{
		tops.translate_line(ass->tc, line);
		iops.destroy_line(ass->ic);
	}

	program_valid 	= tops.is_program_valid(ass->tc);
	finalize_result = tops.finalize(ass->tc);

	if (!program_valid ||
	    finalize_result != TRANSLATE_SUCCESS || !oops.dump(ass->oc))
	{
		printf("In file %s:\n", real_file_name);
		__print_errors();
	}

assembler_process_exit:
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