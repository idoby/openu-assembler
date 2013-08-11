static int __finalize_check_labels(table_element *elem, void *arg)
{
	symbol *sym = table_entry(elem);
	list *err_list = arg;

	if (!table_is_defined(sym))
	{
		error *err = error_make(ERROR_NO_LINE, "Label '%s' is undefined!", sym->name);
		list_insert_before(err_list, &err->errors);
		return 0;
	}

	return 1;
}

static void __finalize_translate_instruction(default_instruction *inst)
{
	UNUSED_PARAM(inst);
}