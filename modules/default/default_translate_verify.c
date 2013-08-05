static const char* __verify_register(const char *p)
{
	if (p[0] == '\0' || p[1] == '\0')
		return NULL;

	if (p[0] == REGISTER_PREFIX && p[0] == REGISTER_PREFIX && p[1] >= '0' && p[1] <= '0' + NUM_REGISTERS - 1)
		return p + REGISTER_NAME_WIDTH;

	return NULL;
}

static const char* __verify_label_aux(const char *p, int optional)
{
	const char *p2 = p;
	ins_prototype *inst;

	/* Does the line begin with the label indicator? That's bad! */
	if (*p2 == LABEL_INDICATOR)
		return NULL;

	/* Or maybe with a non-alphanumeric character? Must mean there is no label! */
	if (!isalpha(*p2))
		return optional ? p : NULL;

	/* Is this the name of a register? */
	if (__verify_register(p))
		return NULL;

	/* If the label is an instruction name, it's invalid, but we might not care about it. */
	for_each_instruction(inst)
		if (strncmp(p, inst->name, strlen(inst->name)) == 0)
			return optional ? p : NULL;

	/* Find the end of the label. */
	while (isalnum(*p2) || *p2 == '_' || *p2 == '-')
		++p2;

	/* Is it too long? */
	if (p2 - p > SYMBOL_MAX_LENGTH)
		return NULL;

	return p2;
}

static const char* __verify_label(const char *p)
{
	return __verify_label_aux(p, 0);
}

static const char* __verify_label_optional(const char* p)
{
	return __verify_label_aux(p, 1);
}

static const char* __verify_number(const char *p)
{
	if (p == NULL)
		return NULL;

	/* A + or a - at the beginning is fine. */
	if (*p == POSITIVE_INDICATOR || *p == NEGATIVE_INDICATOR)
		++p;

	/* There must be at least one digit. */
	if (!isdigit(*p))
		return NULL;

	while (isdigit(*p))
		++p;

	return p;
}

static const char* __verify_string(const char *p)
{
	if (p == NULL)
		return NULL;

	/* Must begin with a ". */
	if (*p++ != STRING_DELIMITER)
		return NULL;

	/* A string contains any printable character but ". */
	/* Tab is allowed in the string too but isn't caught by isprint(). */
	while ((isprint(*p) || *p == '\t') && *p != STRING_DELIMITER)
		++p;

	/* It then must end with another ". */
	if (*p++ != STRING_DELIMITER)
		return NULL;

	/* Otherwise this is a valid string. */
	return p;
}

static const char* __verify_list(const char *p, const char* (*consume)(const char *p))
{
	/* A blank must follow the directive token. */
	if (!__isblank(*p))
		return NULL;

	while (1)
	{
		/* After that we can have as much WS as we want, */
		/* followed by a valid token and more WS. */
		p = __skip_whitespace(consume(__skip_whitespace(p)));

		/* If we didn't get a valid number, abort.*/
		if (p == NULL)
			return NULL;

		/* If a comma is present, we need to consume one more number. */
		if (*p != SEPARATOR || __islineterm(*p))
			break;

		/* Consume the comma. */
		++p;
	}

	/*if (!__islineterm(*p))
		return NULL;*/

	return p;
}

static const char* __verify_directive(const char *p)
{
#define is_directive(p,d) (strncmp((p), (d), strlen((d))) == 0)

	if (is_directive(p, DIRECTIVE_DATA))
	{
		/* Skip the directive name. */
		p += strlen(DIRECTIVE_DATA);
		return __verify_list(p, __verify_number);
	}
	else if (is_directive(p, DIRECTIVE_STRING))
	{
		p += strlen(DIRECTIVE_STRING);
		return __verify_list(p, __verify_string);
	}
	else if (is_directive(p, DIRECTIVE_EXTERN))
	{
		p += strlen(DIRECTIVE_EXTERN);
		return __verify_list(p, __verify_label);
	}
	else if (is_directive(p, DIRECTIVE_ENTRY))
	{
		p += strlen(DIRECTIVE_ENTRY);
		return __verify_list(p, __verify_label);
	}

	return NULL; /* This is not a valid directive line. */
#undef is_directive
}

static const char* __verify_modifiers(const char *p, ins_prototype *proto)
{
	p = __skip_whitespace(p);

	/* Did we find the /? */
	if (*p++ != INST_MOD_DELIMITER)
		return NULL;

	p = __skip_whitespace(p);

	if (*p == INST_MOD_TYPE_TRUE)	/* type=1, so we need to find a /# sequence for each operand */
	{
		unsigned int operand = 0;

		++p;

		for (; operand < proto->num_operands; ++operand)		
		{
			p = __skip_whitespace(p);
			if (*p++ != INST_MOD_DELIMITER) /* Find the /. */
				return NULL;
			p = __skip_whitespace(p);

			if (*p != INST_MOD_LEFT_BITS && *p != INST_MOD_RIGHT_BITS) /* Must be a 0 or a 1 here. */
				return NULL;

			++p;
		}
	}
	else if (*p++ != INST_MOD_TYPE_FALSE) /* Must be type=0 here or error. */
		return NULL;

	p = __skip_whitespace(p);

	/* After the type specifier we have to find a , and a dbl symbol. */
	if (*p++ != SEPARATOR)
		return NULL;

	p = __skip_whitespace(p);

	if (*p != INST_MOD_DBL_TRUE && *p != INST_MOD_DBL_FALSE)
		return NULL;

	return ++p;
}

static const char* __verify_operand(const char* p, unsigned int allowed)
{
	/* Check if immediate address. */
	if (*p == IMMEDIATE_INDICATOR && (allowed & IMMEDIATE) != 0)
		return __verify_number(__skip_whitespace(++p));
	/* Check if register name. */
	else if (__verify_register(p) != NULL && (allowed & REGISTER) != 0)
		return __verify_register(p);
	/* The only thing we have left is a label or an index. */
	else if ((p = __verify_label(p)) != NULL)
	{
		if (*p == INDEX_START) /* An index. */
		{
			if ((allowed & INDEX) == 0)
				return NULL;

			++p;

			if (*p == INDEX_LABEL_INDICATOR) /* An index with a label offset. */
			{
				++p;
				if ((p = __verify_label(p)) == NULL)
					return NULL;
			}
			else if (__verify_register(p) != NULL) /* An index with a register offset. */
				p = __verify_register(p);
			else if ((p = __verify_number(p)) == NULL) /* Otherwise this leaves a number. */
				return NULL;

			/* The index part must end with a }. */
			if (*p != INDEX_END)
				return NULL;

			return ++p;
		}
		else
			return p;
	}

	/* If none of the above are valid, this is not a valid operand. */
	return NULL;
}

static const char* __verify_operands(const char *p, ins_prototype *proto)
{
	unsigned int operand = 0;

	for (; operand < proto->num_operands; ++operand)
	{
		/*	Verify one operand, according to the allowed types in proto,
			surrounded by whitespace. */
		p = __skip_whitespace(__verify_operand(__skip_whitespace(p), proto->allowed_modes[operand]));

		if (p == NULL)
			return NULL;

		/* If this is the last operand, we don't need to consume more. */
		if (operand == proto->num_operands - 1)
			break;

		/* Otherwise, we require a comma to separate the operands. */
		if (*p != SEPARATOR)
			return NULL;

		/* Consume the comma. */
		++p;
	}

	return p;
}

static const char* __verify_instruction(const char *p)
{
	ins_prototype *inst = inst_prototypes;

	for_each_instruction(inst)
		if (strncmp(p, inst->name, strlen(inst->name)) == 0)
		{
			/* Skip the instruction name. */
			p += strlen(inst->name);

			/* Verify the modifiers after the instruction name. */
			p = __verify_modifiers(p, inst);
			
			if (p == NULL)
				return NULL;

			/*	The hardest part of the verification.
				The last bastion of unverified syntax lies beyond p.
				Let us go boldly now where no pointer has gone before.
				Some of us may not return, but it must be done. */
			return __verify_operands(p, inst);
		}
	
	/* This is not an instruction we know, therefore it is invalid. */
	return NULL;
}

static translate_line_error __verify_line(default_translate_context *tc, const char *line)
{
	const char *p = line;
	const char *q;

	p = __skip_whitespace(p);

	/* A digit at the start of the line is never valid in this language. */
	if (isdigit(*p))
		return TRANSLATE_LINE_SYNTAX_ERROR; /* TODO: error message. */

	/* An empty or comment line is meaningless, but it is valid. */
	if (__islineterm(*p))
		return TRANSLATE_LINE_SUCCESS;

	/* Is there a valid label at the start of the line? */
	q = __verify_label_optional(p);

	/* Bad label. */
	if (q == NULL)
		return TRANSLATE_LINE_SYNTAX_ERROR; /* TODO: error message. */

	/* If there is a label, skip it. */
	if (q != p)
	{
		/* Skip the label name. */
		p = q;

		/* At this point in the parse, the label must be followed by a ':'. */
		if (*p != LABEL_INDICATOR)
			return TRANSLATE_LINE_SYNTAX_ERROR; /* TODO: error message. */

		/* Skip the ':'. */
		++p;
	}

	p = __skip_whitespace(p);

	/* A directive may now follow. */
	if (*p == DIRECTIVE_START)
	{
		++p;

		/*	This function verifies rest of the line as a directive. */
		if ((p = __verify_directive(p)) == NULL)
			return TRANSLATE_LINE_SYNTAX_ERROR; /* TODO: error message. */
	}
	else if ((p = __verify_instruction(p)) == NULL)
		return TRANSLATE_LINE_SYNTAX_ERROR; /* TODO: error message. */

	return __islineterm(*p) ? TRANSLATE_LINE_SUCCESS : TRANSLATE_LINE_SYNTAX_ERROR; /* TODO: error message. */
}