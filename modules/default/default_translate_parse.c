typedef enum parse_symbol_error {
	LABEL_EXISTS,
	LABEL_ALLOC_ERROR,
	LABEL_SET,
	LABEL_NO_LABEL
} parse_symbol_error;

static parse_symbol_error __parse_assign_symbol(scratch_space *s, symbol_table *syms, const char* label, symbol_type type, unsigned int define)
{
	symbol *old_sym = NULL;
	symbol *new_sym = NULL;

	if (label == NULL)
		return LABEL_NO_LABEL;

	old_sym = table_find_symbol(syms, label);
	if (old_sym != NULL)
	{
		if (!table_is_defined(old_sym))
			new_sym = old_sym; /* This refers to an old undefined symbol, so we'll define it. */
		else
			return LABEL_EXISTS;
	}

	/*	Do we already have a symbol to work on?
		If not, define a new one. */
	if (new_sym == NULL)
		new_sym = table_new_symbol(syms, label);

	if (new_sym == NULL)
		return LABEL_ALLOC_ERROR;

	if (s != NULL)
		table_set_address_space(new_sym, s, scratch_get_next_offset(s));

	if ((define && !table_is_entry(new_sym)) || type == ENTRY)
		table_set_type(new_sym, type);

	if (define)
		table_set_defined(new_sym);

	return LABEL_SET;
}

static symbol* __parse_get_symbol(symbol_table *syms, const char *label)
{
	symbol *sym = table_find_symbol(syms, label);

	if (sym != NULL)
		return sym;

	return table_new_symbol(syms, label);
}

static int __parse_define_label(scratch_space *s, default_translate_context *dtc, const char *label)
{
	error *err = NULL;
	switch (__parse_assign_symbol(s, dtc->syms, label, INTERN, 1))
	{
		case LABEL_EXISTS:
			err = error_make(dtc->line_number, "Label %s is being defined twice", label);
			list_insert_before(dtc->errors, &err->errors);
			return 0;
			break;
		case LABEL_ALLOC_ERROR:
			err = error_make(dtc->line_number, "Unable to allocate space for label %s, quitting", label);
			list_insert_before(dtc->errors, &err->errors);
			return 0;
			break;
		case LABEL_NO_LABEL:
		case LABEL_SET:
		default:
			return 1; /* Everything is fine. */
	}
}

static parse_symbol_error __parse_label(const char *p, char out_sym[SYMBOL_MAX_LENGTH + 1])
{
	unsigned int index = 0;
	const char* p2 = p;

	/* Find the end of the label and copy it. */
	for (; (isalnum(*p2) || *p2 == '_' || *p2 == '-') && index < SYMBOL_MAX_LENGTH + 1; ++index, ++p2)
		out_sym[index] = *p2;

	out_sym[index] = '\0';

	/* Do we have a label? */
	if (*p2 == LABEL_INDICATOR)
	{
	 	return LABEL_EXISTS;
	}

	return LABEL_NO_LABEL;
}

static const char* __parse_number(long int *num, const char *p)
{
	char *data_end = NULL;

	/* Parse data item. */
	*num = strtol(p, &data_end, 10);

	return data_end;
}

static const char* __parse_register(int *reg_num, const char *p)
{
	*reg_num = INVALID_REGISTER;

	if (p[0] == '\0' || p[1] == '\0')
		return p;

	if (is_valid_register(p))
	{
		*reg_num = p[1] - REGISTER_FIRST_NUMBER;
		return p + REGISTER_NAME_WIDTH;
	}

	return p;
}

static const char* __parse_number_list(scratch_space *s, const char *p)
{
	while (1)
	{
		long int num = 0;

		p = __skip_whitespace(__parse_number(&num, p));
		scratch_write_next_data(s, num, ABSOLUTE);

		/* If a comma is present, we need to consume one more number. */
		if (*p != SEPARATOR || __islineterm(*p))
			return p;

		++p;
	}
}

static const char* __parse_string_list(scratch_space *s, const char *p)
{
	while (1)
	{
		p = __skip_whitespace(p) + 1; /* Skip the " and whitespace. */

		/* Parse list item. */
		while (__isstringchar(*p) && *p != STRING_DELIMITER)
		{
			scratch_write_next_data(s, *p, ABSOLUTE);
			++p;
		}

		/* Write a "null terminator" for the string. */
		scratch_write_next_data(s, '\0', ABSOLUTE);

		p = __skip_whitespace(p + 1); /* Skip the " and whitespace. */

		/* If a comma is present, we need to consume one more number. */
		if (*p != SEPARATOR || __islineterm(*p))
			return p;

		++p;
	}
}

static const char* __parse_label_list(default_translate_context *dtc, const char *p, symbol_type type)
{
	error *err = NULL;
	while (1)
	{
		char label[SYMBOL_MAX_LENGTH + 1] = {0};

		p = __skip_whitespace(p);

		/* Copy out label name. */
		__parse_label(p, label);

		/* 	Define the label, or quit if unsuccessful.
		 	The label should be defined only if it's external.
		 	Entry labels still require a definition in the file. */
		switch (__parse_assign_symbol(NULL, dtc->syms, label, type, type == EXTERN))
		{
			case LABEL_ALLOC_ERROR:
				err = error_make(dtc->line_number, "Unable to allocate space for label %s, quitting", label);
				list_insert_before(dtc->errors, &err->errors);
				return NULL;
				break;
			case LABEL_EXISTS:
				err = error_make(dtc->line_number, "Label %s is being defined twice", label);
				list_insert_before(dtc->errors, &err->errors);
				return NULL;
				break;
			case LABEL_NO_LABEL:
			case LABEL_SET:
			default:
				break; /* Everything is fine. */
		}

		p = __skip_whitespace(p + strlen(label));

		/* If a comma is present, we need to consume one more number. */
		if (*p != SEPARATOR || __islineterm(*p))
			return p;

		++p;
	}
}

/*	The following functions are the meat of this project.
	It is not surprising, therefore, that they were written as late as possible. */
static const char* __parse_directive(default_translate_context *dtc, const char *p, const char *label)
{
	if (is_directive(p, DIRECTIVE_DATA))
	{
		/* Skip the directive name. */
		p = __skip_whitespace(p + strlen(DIRECTIVE_DATA));

		/* If a label exists, it should point to the start of the data. */
		if (!__parse_define_label(dtc->d_scratch, dtc, label))
			return NULL;

		/* Parse the list and write it to the data space. */
		return __parse_number_list(dtc->d_scratch, p);
	}
	else if (is_directive(p, DIRECTIVE_STRING))
	{
		p = __skip_whitespace(p + strlen(DIRECTIVE_STRING));

		if (!__parse_define_label(dtc->d_scratch, dtc, label))
			return NULL;

		return __parse_string_list(dtc->d_scratch, p);
	}
	else if (is_directive(p, DIRECTIVE_EXTERN))
	{
		p = __skip_whitespace(p + strlen(DIRECTIVE_EXTERN));
		return __parse_label_list(dtc, p, EXTERN);
	}
	else if (is_directive(p, DIRECTIVE_ENTRY))
	{
		p = __skip_whitespace(p + strlen(DIRECTIVE_ENTRY));
		return __parse_label_list(dtc, p, ENTRY);
	}

	return NULL; /* All paths must return to keep the compiler happy. */
}

static const char* __parse_modifiers(const char *p, default_instruction *inst)
{
	/* Skip the /. */
	p = __skip_whitespace(p + 1);

	if (*p == INST_MOD_TYPE_TRUE)	/* type=1, so we need to find a /# sequence for each operand */
	{
		unsigned int operand = 0;

		inst->type = 1;
		++p;

		for (; operand < MAX_OPERANDS; ++operand)		
		{
			/* Skip the /. */
			p = __skip_whitespace(p + 1);

			if (*p == INST_MOD_LEFT_BITS)
				inst->comb |= 0;
			else if(*p == INST_MOD_RIGHT_BITS)
				inst->comb |= 1;

			++p;

			if (operand != inst->proto->num_operands - 1)
				inst-> comb <<= 1;
		}
	}
	else if (*p++ == INST_MOD_TYPE_FALSE) /* Must be type=0. */
		inst->type = 0;

	/* After the type specifier we have to find a , and a dbl symbol. */
	p = __skip_whitespace(__skip_whitespace(p) + 1);

	if (*p == INST_MOD_DBL_TRUE)
		inst->dbl = 1;
	else if (*p == INST_MOD_DBL_FALSE)
		inst->dbl = 0;

	return ++p;
}

static const char* __parse_operand(default_translate_context *dtc, const char *p, address *ad)
{
	error *err = NULL;
	int reg_num = INVALID_REGISTER;
	scratch_space *is = dtc->i_scratch;

	if (*p == IMMEDIATE_INDICATOR)
	{
		long int num = 0;

		/* Skip the # and parse the number. */
		p = __skip_whitespace(__parse_number(&num, p + 1));

		default_address_set_immediate(ad, num);

		/* Write the number to reserve space. */
		scratch_write_next_data(is, num, ABSOLUTE);

		return p;
	}

	p = __parse_register(&reg_num, p);

	/* We have a register name. */
	if (reg_num != INVALID_REGISTER)
	{
		default_address_set_register(ad, reg_num);
		return p;
	}
	else
	{
		/* Otherwise, a label name must follow. */
		char first_label[SYMBOL_MAX_LENGTH + 1] = {0};
		unsigned int first_label_off = 0;
		symbol *first_sym = NULL;

		__parse_label(p, first_label);
		p = __skip_whitespace(p + strlen(first_label));

		/* Find or create a new symbol. */
		first_sym = __parse_get_symbol(dtc->syms, first_label);

		if (first_sym == NULL)
		{
			err = error_make(dtc->line_number, "Unable to allocate space for label %s, quitting", first_label);
			list_insert_before(dtc->errors, &err->errors);
			return NULL;
		}

		default_address_set_symbol(ad, first_sym);

		/*	Mark this instruction as a reference to the symbol
			and write some zeroes to reserve space for this symbol. */
		first_label_off = scratch_get_global_offset(is, scratch_get_next_offset(is));
		table_add_reference(first_sym, first_label_off);
		scratch_write_next_data(is, 0, ABSOLUTE);

		/* Do we have an index or just a symbol address? */
		if (*p != INDEX_START)
			return p;

		p = __skip_whitespace(p + 1);

		/* If the next character is a digit, a + or a -, this is a number. */
		if (isdigit(*p) || *p == POSITIVE_INDICATOR || *p == NEGATIVE_INDICATOR)
		{
			long int num = 0;

			/* Skip the # and parse the number. */
			p = __parse_number(&num, p);

			default_address_set_index_number(ad, num);

			/* Write the number to reserve space. */
			scratch_write_next_data(is, num, ABSOLUTE);
		}
		else
		{
			p = __parse_register(&reg_num, p);

			/* We have a register. */
			if (reg_num != INVALID_REGISTER)
				default_address_set_index_register(ad, reg_num);
			else
			{
				char sec_label[SYMBOL_MAX_LENGTH + 1] = {0};
				unsigned int sec_label_off = 0;
				symbol *sec_sym = NULL;

				/*	Otherwise this must be a label.
					Skip the *. */
				p = __skip_whitespace(p + 1);

				__parse_label(p, sec_label);
				p = __skip_whitespace(p + strlen(sec_label));

				/* Find or create a new symbol. */
				sec_sym = __parse_get_symbol(dtc->syms, sec_label);

				if (sec_sym == NULL)
				{
					err = error_make(dtc->line_number, "Unable to allocate space for label %s, quitting", sec_label);
					list_insert_before(dtc->errors, &err->errors);
					return NULL;
				}

				default_address_set_index_symbol(ad, sec_sym);

				/*	Mark this instruction as a reference to the symbol
				and write some zeroes to reserve space for this symbol. */
				sec_label_off = scratch_get_global_offset(is, scratch_get_next_offset(is));
				table_add_reference(sec_sym, sec_label_off);
				scratch_write_next_data(is, 0, ABSOLUTE);
			}
		}

		/* Skip the }. */
		return __skip_whitespace(p) + 1;
	}
}

static const char* __parse_operands(default_translate_context *dtc, const char *p, default_instruction *inst)
{
	unsigned int operand = 0;

	for (; operand < inst->proto->num_operands; ++operand)
	{
		/*	Verify one operand, according to the allowed types in proto,
			surrounded by whitespace. */
		p = __skip_whitespace(p);
		p = __skip_whitespace(__parse_operand(dtc, p, &inst->operands[operand]));

		if (p == NULL)
			return NULL;

		/* Consume the comma. */
		if (operand != inst->proto->num_operands - 1)
			++p;
	}

	return p;
}

static const char* __parse_instruction(default_translate_context *dtc, const char *p, const char *label)
{
	default_instruction *inst = NULL;

	p = __skip_whitespace(p);

	inst = default_instruction_make(p);

	if (inst == NULL)
	{
		error *err = error_make(dtc->line_number, "Unable to allocate space for instruction, quitting");
		list_insert_before(dtc->errors, &err->errors);
		return NULL;
	}

	/* Prepare space in the address space for the instruction. */
	inst->address_space = dtc->i_scratch;
	inst->address_offset = scratch_get_next_offset(dtc->i_scratch);

	/* Define a label for this line if it exists. */
	if (!__parse_define_label(dtc->i_scratch, dtc, label))
		goto parse_exit;

	/* Write zeroes for now just to reserve space. */
	scratch_write_next_data(dtc->i_scratch, 0, ABSOLUTE);

	p = __skip_whitespace(p + strlen(inst->proto->name));

	p = __parse_modifiers(p, inst);
	p = __parse_operands(dtc, p, inst);

	if (p == NULL)
		goto parse_exit;

	/* Add new instruction to the end of the instruction list in the context. */
	list_insert_before(&dtc->insts, &inst->insts);

	return p;

parse_exit:
	default_instruction_destroy(inst);
	return NULL;
}

/*	Breaks down the line into a more abstract representation, assuming that the syntax
	is valid. */
static translate_line_error __parse_line(default_translate_context *dtc, const char *p)
{
#define LABEL_IF_EXISTS (line_label_exists == LABEL_EXISTS ? line_label : NULL)
	const char line_label[SYMBOL_MAX_LENGTH + 1] = {0};
	parse_symbol_error line_label_exists = LABEL_NO_LABEL;

	p = __skip_whitespace(p);

	/* If the line is empty or a comment, we don't need to process it. */
	if (__islineterm(*p))
		return TRANSLATE_LINE_SUCCESS;

	/* Is there a valid label at the start of the line? */
	line_label_exists = __parse_label(p, (char*)line_label);

	/* Skip the label and the :. */
	if (line_label_exists == LABEL_EXISTS)
		p += strlen(line_label) + 1;

	p = __skip_whitespace(p);

	/* A directive may now follow. */
	if (*p == DIRECTIVE_START)
	{
		if (__parse_directive(dtc, ++p, LABEL_IF_EXISTS) == NULL)
			return TRANSLATE_LINE_ALLOC_ERROR;
	}
	/* Otherwise the line must be an instruction. */
	else if (__parse_instruction(dtc, p, LABEL_IF_EXISTS) == NULL)
		return TRANSLATE_LINE_ALLOC_ERROR;

	return TRANSLATE_LINE_SUCCESS;

#undef LABEL_IF_EXISTS
}