#include <stdlib.h>
#include <string.h>

#include "default_translate.h"

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
		struct index_s
		{
			struct symbol *symbol;
			struct address *index;
		} index;
		unsigned int register_number;
	} data;
} address;

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

static void __address_destroy(address *ad)
{
	if (ad == NULL)
		return;

	if (ad->type == INDEX && ad->data.index.index != NULL)
		__address_destroy(ad->data.index.index);

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