#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "default_translate.h"

#include "intrusive_list.h"
#include "scratch_space.h"
#include "symbol_table.h"

enum addressing_mode {
	IMMEDIATE 	= 1,
	DIRECT		= 2,
	INDEX		= 4,
	REGISTER	= 8,
	ALL = IMMEDIATE | DIRECT | INDEX | REGISTER,
	NO_IMMEDIATE = DIRECT | INDEX | REGISTER,
	NONE = 0,
	ADDRESSING_MAX_MODE = REGISTER
};

typedef struct default_translate_context {
	list *insts;
	symbol_table *syms;
	scratch_space *i_scratch;
	scratch_space *d_scratch;
} default_translate_context;

translate_ops default_translate_ops = 
				{	default_translate_init,
					default_translate_destroy,
					default_translate_line,
					default_translate_finalize };

#define INSTRUCTION_NAME_MAX_LEN 	4

#define INSTRUCTION_LIST(list_entry)	\
					/*name	opcode	#operands	src allowed modes	dst allowed modes. */	\
	list_entry		(mov,	00,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(cmp,	01,		2,			ALL,				ALL			)			\
	list_entry		(add,	02,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(sub,	03,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(not,	04,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(clr,	05,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(lea,	06,		2,			NO_IMMEDIATE,		NO_IMMEDIATE)			\
	list_entry		(inc,	07,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(dec,	010,	1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(jmp,	011,	1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(bne,	012,	1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(red,	013,	1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(prn,	014,	1,			NONE,				ALL			)			\
	list_entry		(jsr,	015,	1,			NONE,				IMMEDIATE	)			\
	list_entry		(rts,	016,	0,			NONE,				NONE		)			\
	list_entry		(stop,	017,	0,			NONE,				NONE		)

#define INS_MAKE_PROTOTYPE(name, opcode, ops, src_modes, dst_modes)	\
	{#name, (opcode), (ops), {(src_modes), (dst_modes)}},

static ins_prototype inst_prototypes[] = { INSTRUCTION_LIST(INS_MAKE_PROTOTYPE) /*,*/ {NULL, 0, 0, {0,0}} }; /* This line is not an error. */

typedef struct address {
	enum addressing_mode type;
	union address_data {
		int immediate_data;
		symbol *direct_sym;
		struct index_data
		{
			struct symbol *symbol;
			struct address *index;
		} index_data;
		unsigned int register_number;
	} data;
} address;

#define PARSE_LOOP_GUARD(p) (*(p) != LINE_END && *(p) != '\0')

static const char LINE_END			= '\n';
static const char COMMENT_START 	= ';';
static const char LABEL_INDICATOR	= ':';
static const char DIRECTIVE_START	= '.';

enum default_label_errors {
	LABEL_NOT_FOUND,
	LABEL_ALLOC_ERROR,
	LABEL_TOO_LONG,
	LABEL_INVALID,
	LABEL_SUCCESS
};

translate_context*	default_translate_init
					(list *insts, symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch)
{
	default_translate_context *dtc;

	if (insts == NULL || syms == NULL || i_scratch == NULL || d_scratch == NULL)
		return NULL;

	if ((dtc = malloc(sizeof(*dtc))) == NULL)
		return NULL;
	
	dtc->insts	= insts;
	dtc->syms	= syms;
	dtc->i_scratch = i_scratch;
	dtc->d_scratch = d_scratch;

	return (translate_context*)dtc;
}

void default_translate_destroy(translate_context *tc)
{
	free(tc);
	tc = tc; /* Nothing to do here, but keep compiler quiet. */
}

static char* __skip_whitespace(char *p)
{
	while (isspace(*p) && PARSE_LOOP_GUARD(p))
		++p;

	return p;
}

static enum default_label_errors __get_label(char *p, char **label)
{
	/* Lookahead to see if we're dealing with a label. */
	char *p2 = p;
	char *lp;

	/* Does the line begin with the label indicator? That's bad! */
	if (*p2 == LABEL_INDICATOR)
		return LABEL_INVALID;

	/* Or maybe with a non-alphanumeric character? Also bad! */
	if (!isalpha(*p2))
		return LABEL_NOT_FOUND;

	/* Find the end of the label. */
	while (isalnum(*p2) && PARSE_LOOP_GUARD(p2))
		++p2;

	/* We've found a label! */
	if (*p2 != LABEL_INDICATOR)
		return LABEL_NOT_FOUND;

	/* Is it too long? */
	if (p2 - p > SYMBOL_MAX_LENGTH)
		return LABEL_TOO_LONG;

	if ((*label = malloc(SYMBOL_MAX_LENGTH + 1)) == NULL)
		return LABEL_ALLOC_ERROR;

	/* Copy the symbol name. */
	for (lp = *label; p != p2; ++p, ++lp)
		*lp = *p;

	/* Add the null terminator. */
	*lp = '\0';

	return LABEL_SUCCESS;
}

/*	The following two functions are the meat of this project.
	It is not surprising, therefore, that they were written as late as possible. */
translate_line_error default_translate_line(translate_context *tc, char *line, unsigned int line_number)
{
	char *p = line;
	char *label = NULL;
	translate_line_error ret_val = TRANSLATE_SUCCESS;

	if (tc == NULL || line == NULL || line_number == 0)
	{
		ret_val = TRANSLATE_LINE_ERROR;
		goto translate_line_exit;
	}

	/* Skip leading whitespace. */
	p = __skip_whitespace(p);

	/* A digit at the start of the line is never valid in this language. */
	if (isdigit(*p))
	{
		ret_val = TRANSLATE_LINE_ERROR;
		goto translate_line_exit;
	}

	/* A comment at the start of the line makes the whole line meaningless, */
	/* but it is valid. */
	if (*p == COMMENT_START)
		goto translate_line_exit;

	switch (__get_label(p, &label))
	{
		case LABEL_SUCCESS:		/* We've found a label! */
			p += strlen(label) + 1;	/* Skip the label. */
			break;
		case LABEL_NOT_FOUND:	/* No label. This is fine. */
			break;
		case LABEL_ALLOC_ERROR:	/* There was an error in allocation. */
		case LABEL_TOO_LONG:	/* If the label is too long, this line is invalid! */
		case LABEL_INVALID:		/* An invalid label was found! */
		default:
			ret_val = TRANSLATE_LINE_ERROR;
			goto translate_line_exit;
	}

	p = __skip_whitespace(p);

	/* TODO: handle directives. */
	/*if (*p == DIRECTIVE_START)*/

translate_line_exit:

	free(label);
	return ret_val;
}

translate_error default_translate_finalize(translate_context *tc)
{
	tc = tc;
	return TRANSLATE_SUCCESS;
}

static struct ins_prototype* __get_prototype(char* name)
{
	struct ins_prototype *insp = inst_prototypes;

	/* Look up the symbol by name. */
	for (;insp->name != NULL; ++insp)
		if (strcmp(name, insp->name) == 0)
			return insp;

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