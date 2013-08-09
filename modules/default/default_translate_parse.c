typedef enum parse_label_error {
	LABEL_EXISTS,
	LABEL_ALLOC_ERROR,
	LABEL_SET,
	LABEL_NO_LABEL
} parse_label_error;

static parse_label_error __parse_assign_label(scratch_space *s, symbol_table *syms, const char* label, symbol_type type, unsigned int defined)
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

	if (!table_is_entry(new_sym))
		table_set_type(new_sym, type);

	if (defined)
		table_set_defined(new_sym);

	return LABEL_SET;
}

static int __parse_define_label(scratch_space *s, symbol_table *syms, const char *label)
{
	switch (__parse_assign_label(s, syms, label, INTERN, 1))
	{
		case LABEL_EXISTS:
			return 0; /* TODO: label is a duplicate, error message. */
			break;
		case LABEL_ALLOC_ERROR:
			return 0; /* TODO: allocation error, message. */
			break;
		case LABEL_NO_LABEL:
		case LABEL_SET:
		default:
			return 1; /* Everything is fine. */
	}
}

static parse_label_error __parse_label(const char *p, char out_sym[SYMBOL_MAX_LENGTH + 1])
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

static const char* __parse_number_list(scratch_space *s, const char *p)
{
	long int data_item = 0;
	char *data_end = NULL;

	while (1)
	{
		/* Parse data item. */
		data_item = strtol(p, &data_end, 10);
		scratch_write_next_data(s, data_item, ABSOLUTE);

		p = __skip_whitespace(data_end);

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

		p = __skip_whitespace(p + 1); /* Skip the " and whitespace. */

		/* If a comma is present, we need to consume one more number. */
		if (*p != SEPARATOR || __islineterm(*p))
			return p;

		++p;
	}
}

static const char* __parse_label_list(default_translate_context *dtc, const char *p, symbol_type type)
{
	while (1)
	{
		char label[SYMBOL_MAX_LENGTH + 1] = {0};

		p = __skip_whitespace(p);

		/* Copy out label name. */
		__parse_label(p, label);

		/* 	Define the label, or quit if unsuccessful.
		 	The label should be defined only if it's external.
		 	Entry labels still require a definition in the file. */
		switch (__parse_assign_label(NULL, dtc->syms, label, type, type == EXTERN))
		{
			case LABEL_ALLOC_ERROR:
				return NULL; /* TODO: error message. */
				break;
			case LABEL_EXISTS:
				return NULL; /* TODO: error message. */
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
		if (!__parse_define_label(dtc->d_scratch, dtc->syms, label))
			return NULL;

		/* Parse the list and write it to the data space. */
		return __parse_number_list(dtc->d_scratch, p);
	}
	else if (is_directive(p, DIRECTIVE_STRING))
	{
		p = __skip_whitespace(p + strlen(DIRECTIVE_STRING));

		if (!__parse_define_label(dtc->d_scratch, dtc->syms, label))
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

static const char* __parse_instruction(default_translate_context *dtc, const char *p, const char *label)
{
	/* TODO: implement. */
	return p;
}

/*	Breaks down the line into a more abstract representation, assuming that the syntax
	is valid. */
static translate_line_error __parse_line(default_translate_context *dtc, const char *p)
{
#define LABEL_IF_EXISTS (line_label_exists == LABEL_EXISTS ? line_label : NULL)
	const char line_label[SYMBOL_MAX_LENGTH + 1] = {0};
	parse_label_error line_label_exists = LABEL_NO_LABEL;

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
		return TRANSLATE_LINE_ALLOC_ERROR; /* TODO: error message. */

	return TRANSLATE_LINE_SUCCESS;

#undef LABEL_IF_EXISTS
}

static ins_prototype* __get_prototype(char* name)
{
	ins_prototype *inst;

	/* Look up the symbol by name. */
	for_each_instruction(inst)
		if (strncmp(name, inst->name, strlen(inst->name)) == 0)
			return inst;

	return NULL;
}

instruction* default_instruction_make(char *name)
{
	instruction *inst;
	unsigned int i = 0;

	struct ins_prototype *proto = __get_prototype(name);

	if (proto == NULL)
		return NULL;

	if ((inst = malloc(sizeof(*inst))) == NULL)
		return NULL;

	inst->proto = proto;
	inst->type 	= 0;
	inst->dbl	= 0;

	for (; i < MAX_OPERANDS; ++i)
		inst->operands[i] = NULL;

	list_init(&inst->insts);

	return inst;
}

static address* __address_make(enum addressing_mode mode)
{
	address *ad;

	if (mode == NONE)
		return NULL;

	/* Hack to test if mode is only one of the valid members of the enum. */
	if (!(mode & (mode - 1)) || mode > ADDRESSING_MAX_MODE)
		return NULL;

	/* Allocate some precious memory. */
	if ((ad = malloc(sizeof(*ad))) == NULL)
		return NULL;

	/* Initialize the addressing type. */
	ad->type = mode;

	/* Initialize the appropriate fields based on the specified mode. */
	switch (mode)
	{
		case IMMEDIATE:
			ad->data.immediate_data		= 0;
			break;
		case DIRECT:
			ad->data.direct_sym			= NULL;
			break;
		case INDEX:
			ad->data.index_data.symbol	= NULL;
			ad->data.index_data.index	= NULL;
			break;
		case REGISTER:
			ad->data.register_number	= 0;
			break;
		default:
			free(ad);	/*	Should never happen, but  */
			ad = NULL;	/*	of course, it's bound to. */
	}

	return ad;
}

static void __address_destroy(address *ad)
{
	if (ad == NULL)
		return;

	if (ad->type == INDEX && ad->data.index_data.index != NULL)
		__address_destroy(ad->data.index_data.index);

	free(ad);
}

void default_instruction_destroy(instruction *inst)
{
	unsigned int i = 0;
	if (inst == NULL)
		return;

	list_remove(&inst->insts);

	for (; i < MAX_OPERANDS; ++i)
		__address_destroy(inst->operands[i]);

	free(inst);
}