#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <data_structures/error.h>

#include "default_output.h"

typedef struct default_output_context {
	char	object_file_name[MAX_FILE_NAME];
	char	extern_file_name[MAX_FILE_NAME];
	char	entry_file_name[MAX_FILE_NAME];
	
	scratch_space 	*i_scratch;
	scratch_space 	*d_scratch;

	symbol_table 	*syms;

	list 			*errors;
} default_output_context;

output_ops default_output_ops = {	default_output_init,
									default_output_dump,
									default_output_destroy};

static const char object_file_extension[] = ".ob";
static const size_t object_file_ext_len = sizeof(object_file_extension) - 1;

static const char extern_file_extension[] = ".ext";
static const size_t extern_file_ext_len = sizeof(extern_file_extension) - 1;

static const char entry_file_extension[] = ".ent";
static const size_t entry_file_ext_len = sizeof(extern_file_extension) - 1;

#define try_open_file(which)	\
	do {	\
			if ((which ## _file = fopen(doc->which ## _file_name, "w")) == NULL) {	\
				/* File opening failed, abort. */	\
				error *err = error_make(ERROR_NO_LINE, "Unable to open file %s", doc->which ## _file_name);	\
				list_insert_before(doc->errors, &err->errors);	\
				success = 0;	\
				goto output_dump_exit;	\
			}	\
		} while(0)

output_context*	default_output_init
				(char* file_name, symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch, list *errors)
{
	default_output_context *doc;

	if (file_name == NULL || i_scratch == NULL || d_scratch == NULL || syms == NULL || errors == NULL)
		return NULL;

	if ((doc = malloc(sizeof(*doc))) == NULL)
		return NULL;

	doc->i_scratch = i_scratch;
	doc->d_scratch = d_scratch;
	doc->syms = syms;
	doc->errors = errors;

	/* Create the filenames of the files we'll be outputting to. */
	strncpy(doc->object_file_name, file_name, MAX_FILE_NAME);
	strncat(doc->object_file_name, object_file_extension, object_file_ext_len);

	strncpy(doc->extern_file_name, file_name, MAX_FILE_NAME);
	strncat(doc->extern_file_name, extern_file_extension, extern_file_ext_len);

	strncpy(doc->entry_file_name, file_name, MAX_FILE_NAME);
	strncat(doc->entry_file_name, entry_file_extension, entry_file_ext_len);

	return doc;
}

static int __output_find_entries(table_element *elem, void *arg)
{
	unsigned int *dump = arg;
	symbol *sym = table_entry(elem);

	/* Check if the symbol is external.
	   if so, we need to output an external file. */
	if (table_is_entry(sym))
		*dump = 1;

	return 1;
}

static int __output_print_entries(table_element *elem, void *arg)
{
	FILE *f = arg;
	symbol *sym = table_entry(elem);
	scratch_space *s = sym->address_space;

	if (table_is_entry(sym))
		fprintf(f, "%s %o\n", sym->name, scratch_get_global_offset(s, sym->address_offset));

	return 1;
}

static int __output_find_externs(table_element *elem, void *arg)
{
	unsigned int *dump = arg;
	symbol *sym = table_entry(elem);

	/* Check if the symbol is external.
	   if so, we need to output an external file. */
	if (table_is_extern(sym))
		*dump = 1;

	return 1;
}

static void __output_print_extern_refs(symbol *sym, unsigned int data, void *arg)
{
	FILE *f = arg;

	fprintf(f, "%s %o\n", sym->name, data);
}

static int __output_print_externs(table_element *elem, void *arg)
{
	symbol *sym = table_entry(elem);

	if (table_is_extern(sym))
		table_consume_references(sym, __output_print_extern_refs, arg);

	return 1;
}

unsigned int default_output_dump(output_context *oc)
{
	default_output_context *doc = oc;
	unsigned int success = 1;
	unsigned int i_words, d_words, dump_externs = 0, dump_entries = 0;
	FILE *object_file = NULL, *extern_file = NULL, *entry_file = NULL;

	if (doc == NULL)
		return 0;

	i_words = scratch_get_next_offset(doc->i_scratch);
	d_words = scratch_get_next_offset(doc->d_scratch);

	/* Check if there are any externs and entries that we need to output. */
	table_traverse(doc->syms, __output_find_externs, &dump_externs);
	table_traverse(doc->syms, __output_find_entries, &dump_entries);

	try_open_file(object);

	/* Print the header line with the word count. */
	fprintf(object_file, "%o %o\n", i_words, d_words);

	/* TODO: dump more data from the scratch spaces. */

	/* Print externs if needed. */
	if (dump_externs)
	{
		try_open_file(extern);
		table_traverse(doc->syms, __output_print_externs, extern_file);
	}

	/* Print entries if needed. */
	if (dump_entries)
	{
		try_open_file(entry);
		table_traverse(doc->syms, __output_print_entries, entry_file);
	}

output_dump_exit:
	if (object_file != NULL)
		fclose(object_file);

	if (extern_file != NULL)
		fclose(extern_file);

	if (entry_file != NULL)
		fclose(entry_file);

	return success;
}

void default_output_destroy(output_context *oc)
{
	default_output_context *doc = oc;

	if (doc == NULL)
		return;

	free(doc);

}
