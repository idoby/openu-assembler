#include <stdlib.h>
#include <string.h>
#include "translate.h"

#define INSTRUCTION_NAME_MAX_LEN 	4

#define INSTRUCTION_LIST(list_entry)	\
					/*name	opcode	#operands	src allowed modes	dst allowed modes. */	\
	list_entry		(mov,	0,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(cmp,	1,		2,			ALL,				ALL			)			\
	list_entry		(add,	2,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(sub,	3,		2,			ALL,				NO_IMMEDIATE)			\
	list_entry		(not,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(clr,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(lea,	1,		2,			NO_IMMEDIATE,		NO_IMMEDIATE)			\
	list_entry		(inc,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(dec,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(jmp,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(bne,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(red,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(prn,	1,		1,			NONE,				ALL			)			\
	list_entry		(jsr,	1,		1,			NONE,				NO_IMMEDIATE)			\
	list_entry		(rts,	1,		0,			NONE,				NONE		)			\
	list_entry		(stop,	1,		0,			NONE,				NONE		)

#define INS_MAKE_PROTOTYPE(name, opcode, ops, src_modes, dst_modes)	\
	{#name, (opcode), (ops), {(src_modes), (dst_modes)}},

enum addressing_mode {
	IMMEDIATE 	= 1,
	DIRECT		= 2,
	INDEX		= 4,
	REGISTER	= 8,
	ALL = IMMEDIATE | DIRECT | INDEX | REGISTER,
	NO_IMMEDIATE = DIRECT | INDEX | REGISTER,
	NONE = ALL,	/* Just so we have a symbol for the table. The value is ignored if the operand doesn't exist. */
	ADDRESSING_MAX_MODE = REGISTER
};

ins_prototype inst_prototypes[] = { INSTRUCTION_LIST(INS_MAKE_PROTOTYPE) {NULL, 0, 0, {0,0}} }; /* This line is not an error. */

struct ins_prototype* get_prototype(char* name)
{
	struct ins_prototype *insp = inst_prototypes;

	/* Look up the symbol by name. */
	for (;insp->name != NULL; ++insp)
		if (strcmp(name, insp->name) == 0)
			return insp;

	return NULL;
}

instruction* make_new_instruction(char *name)
{
	instruction *inst;

	struct ins_prototype *proto = get_prototype(name);

	if (proto == NULL)
		return NULL;

	if ((inst = malloc(sizeof(*inst))) == NULL)
		return NULL;

	inst->proto = proto;

	return inst;
}