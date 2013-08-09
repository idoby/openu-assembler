#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "default_translate.h"

#include <data_structures/intrusive_list.h>
#include <data_structures/scratch_space.h>
#include <data_structures/symbol_table.h>
#include <utils.h>

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
	unsigned int 	line_number;
	unsigned int 	program_valid;
	list 			insts;
	symbol_table 	*syms;
	scratch_space 	*i_scratch;
	scratch_space 	*d_scratch;
} default_translate_context;

translate_ops default_translate_ops = 
				{	default_translate_init,
					default_translate_destroy,
					default_translate_line,
					default_is_program_valid,
					default_translate_finalize };

#define INSTRUCTION_NAME_MAX_LEN 	4

#define INSTRUCTION_LIST(list_entry)	\
					/*name	opcode	#operands	1st allowed modes	2nd allowed modes. */	\
	list_entry		(mov,	00,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(cmp,	01,		2,			ALL,				ALL			)			\
	list_entry		(add,	02,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(sub,	03,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(not,	04,		1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(clr,	05,		1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(lea,	06,		2,			NO_IMMEDIATE,		NO_IMMEDIATE)			\
	list_entry		(inc,	07,		1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(dec,	010,	1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(jmp,	011,	1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(bne,	012,	1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(red,	013,	1,			NO_IMMEDIATE,		NONE		)			\
	list_entry		(prn,	014,	1,			ALL,				NONE		)			\
	list_entry		(jsr,	015,	1,			IMMEDIATE,			NONE		)			\
	list_entry		(rts,	016,	0,			NONE,				NONE		)			\
	list_entry		(stop,	017,	0,			NONE,				NONE		)

#define for_each_instruction(inst) for ((inst) = inst_prototypes; (inst)->name != NULL; ++inst)

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

static const int  NUM_REGISTERS			= 8;
static const int  REGISTER_NAME_WIDTH	= 2;
static const char REGISTER_PREFIX		= 'r';
static const char LINE_END				= '\n';
static const char COMMENT_START 		= ';';
static const char LABEL_INDICATOR		= ':';
static const char DIRECTIVE_START		= '.';
static const char SEPARATOR				= ',';
static const char IMMEDIATE_INDICATOR	= '#';
static const char INDEX_START			= '{';
static const char INDEX_END				= '}';
static const char INDEX_LABEL_INDICATOR	= '*';

static const char POSITIVE_INDICATOR	= '+';
static const char NEGATIVE_INDICATOR	= '-';

static const char STRING_DELIMITER		= '"';
static const char INST_MOD_DELIMITER	= '/';

static const char INST_MOD_DBL_TRUE		= '1';
static const char INST_MOD_DBL_FALSE	= '0';

static const char INST_MOD_TYPE_TRUE	= '1';
static const char INST_MOD_TYPE_FALSE	= '0';
static const char INST_MOD_RIGHT_BITS	= '1';
static const char INST_MOD_LEFT_BITS	= '0';

static const char DIRECTIVE_DATA[]		= "data";
static const char DIRECTIVE_STRING[]	= "string";
static const char DIRECTIVE_ENTRY[]		= "entry";
static const char DIRECTIVE_EXTERN[]	= "extern";

#define is_directive(p,d) (strncmp((p), (d), strlen((d))) == 0)

translate_context*	default_translate_init
					(symbol_table *syms, scratch_space *i_scratch, scratch_space *d_scratch)
{
	default_translate_context *dtc;

	if (syms == NULL || i_scratch == NULL || d_scratch == NULL)
		return NULL;

	if ((dtc = malloc(sizeof(*dtc))) == NULL)
		return NULL;
	
	dtc->line_number 	= 0;
	dtc->program_valid	= 1;
	dtc->syms	= syms;
	dtc->i_scratch = i_scratch;
	dtc->d_scratch = d_scratch;

	list_init(&dtc->insts);

	return (translate_context*)dtc;
}

void default_translate_destroy(translate_context *tc)
{
	instruction *current, *safe;
	default_translate_context *dtc = tc;

	if (dtc == NULL)
		return;

	/* Delete all instructions. */
	list_for_each_entry_safe(&dtc->insts, current, safe, instruction, insts)
		default_instruction_destroy(current);

	free(dtc);
}

int __isblank(const int c)
{
	return (c == '\t' || c == ' ');
}

int __islineterm(const int c)
{
	/* A line can be terminated by a \n, a \0 or a ;. */
	return (c == LINE_END || c == '\0' || c == COMMENT_START);
}

int __isstringchar(const int c)
{
	return (isprint(c) || c == '\t');
}

static const char* __skip_whitespace(const char *p)
{
	if (p == NULL)
		return NULL;

	while (__isblank(*p))
		++p;

	return p;
}

#include "default_translate_verify.c"
#include "default_translate_parse.c"

translate_line_error default_translate_line(translate_context *tc, const char *line)
{
	default_translate_context *dtc = tc;

	if (dtc == NULL || line == NULL)
		return TRANSLATE_LINE_BAD_PARAMS;

	++dtc->line_number;

	if (__verify_line(dtc, line) == TRANSLATE_LINE_SYNTAX_ERROR)
	{
		dtc->program_valid = 0;
		return TRANSLATE_LINE_SYNTAX_ERROR;
	}

	/* If the program is no longer valid, we don't parse anymore. */
	if (default_is_program_valid(tc))
		return __parse_line(dtc, line);

	return TRANSLATE_LINE_SUCCESS;
}

unsigned int default_is_program_valid(translate_context *tc)
{
	default_translate_context *dtc = tc;

	if (dtc == NULL)
		return 0;

	return dtc->program_valid;
}

translate_error default_translate_finalize(translate_context *tc)
{
	default_translate_context *dtc = tc;

	if (dtc == NULL)
		return TRANSLATE_BAD_PARAMS;

	if (!default_is_program_valid(tc))
		return TRANSLATE_BAD_PROGRAM;

	/* TODO: implement this function. */

	return TRANSLATE_SUCCESS;
}