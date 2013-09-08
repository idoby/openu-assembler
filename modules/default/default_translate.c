#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "default_translate.h"
#include "default_instruction.h"

#include <data_structures/intrusive_list.h>
#include <data_structures/scratch_space.h>
#include <data_structures/symbol_table.h>
#include <utils.h>

typedef struct default_translate_context {
	unsigned int    line_number;
	unsigned int    program_valid;
	list            insts;
	symbol_table    *syms;
	scratch_space   *i_scratch;
	scratch_space   *d_scratch;
	list            *errors;
} default_translate_context;

translate_ops default_translate_ops = 
             {default_translate_init,
              default_translate_destroy,
              default_translate_line,
              default_is_program_valid,
              default_translate_finalize};

#define INSTRUCTION_SPACE_BEGIN	100

static const int  NUM_REGISTERS         = 8;
static const int  REGISTER_NAME_WIDTH   = 2;
static const char REGISTER_PREFIX       = 'r';
static const char REGISTER_FIRST_NUMBER = '0';
static const char LINE_END              = '\n';
static const char COMMENT_START         = ';';
static const char LABEL_INDICATOR       = ':';
static const char DIRECTIVE_START       = '.';
static const char SEPARATOR             = ',';
static const char IMMEDIATE_INDICATOR   = '#';
static const char INDEX_START           = '{';
static const char INDEX_END             = '}';
static const char INDEX_LABEL_INDICATOR = '*';

static const char POSITIVE_INDICATOR    = '+';
static const char NEGATIVE_INDICATOR    = '-';

static const char STRING_DELIMITER      = '"';
static const char INST_MOD_DELIMITER    = '/';

static const char INST_MOD_DBL_TRUE     = '1';
static const char INST_MOD_DBL_FALSE    = '0';

static const char INST_MOD_TYPE_TRUE    = '1';
static const char INST_MOD_TYPE_FALSE   = '0';
static const char INST_MOD_RIGHT_BITS   = '1';
static const char INST_MOD_LEFT_BITS    = '0';

static const char DIRECTIVE_DATA[]      = "data";
static const char DIRECTIVE_STRING[]    = "string";
static const char DIRECTIVE_ENTRY[]     = "entry";
static const char DIRECTIVE_EXTERN[]    = "extern";

#define is_directive(p,d) (strncmp((p), (d), strlen((d))) == 0)
#define is_valid_register(p) ((p)[0] == REGISTER_PREFIX && \
                              (p)[1] >= REGISTER_FIRST_NUMBER && \
                              (p)[1] <= REGISTER_FIRST_NUMBER + NUM_REGISTERS - 1)

translate_context* default_translate_init
                   (symbol_table *syms,
                    scratch_space *i_scratch, scratch_space *d_scratch,
                    list *errors)
{
	default_translate_context *dtc;

	if (syms == NULL || i_scratch == NULL || d_scratch == NULL || errors == NULL)
		return NULL;

	if ((dtc = malloc(sizeof(*dtc))) == NULL)
		return NULL;
	
	dtc->line_number   = 0;
	dtc->program_valid = 1;
	dtc->syms          = syms;

	dtc->i_scratch = i_scratch;
	scratch_set_global_offset(i_scratch, INSTRUCTION_SPACE_BEGIN);

	dtc->d_scratch = d_scratch;
	dtc->errors    = errors;

	list_init(&dtc->insts);

	return (translate_context*)dtc;
}

void default_translate_destroy(translate_context *tc)
{
	default_instruction *current, *safe;
	default_translate_context *dtc = tc;

	if (dtc == NULL)
		return;

	/* Delete all instructions. */
	list_for_each_entry_safe(&dtc->insts, current, safe, default_instruction, insts)
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
#include "default_translate_finalize.c"

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
	default_instruction *inst;
	unsigned int offset = 0;

	if (dtc == NULL)
		return TRANSLATE_BAD_PARAMS;

	if (!default_is_program_valid(tc))
		return TRANSLATE_BAD_PROGRAM;

	/* Check that all symbols are properly defined (or set to external). */
	if (!table_traverse(dtc->syms, __finalize_check_labels, dtc->errors))
		return TRANSLATE_CANT_RESOLVE;

	/* Set the offset to the data space to be one past
	   the end of the instruction space. */
	offset = scratch_to_global(dtc->i_scratch, scratch_offset(dtc->i_scratch));
	scratch_set_global_offset(dtc->d_scratch, offset);

	/* Rewind the scratch space so that we can do the
	   proper instruction translation pass. */
	scratch_rewind(dtc->i_scratch);

	list_for_each_entry(&dtc->insts, inst, default_instruction, insts)
		__finalize_translate_instruction(dtc, inst);

	return TRANSLATE_SUCCESS;
}