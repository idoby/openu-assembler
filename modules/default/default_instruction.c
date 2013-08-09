#include "default_instruction.h"

#include <string.h>
#include <stdlib.h>

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

#define INS_MAKE_PROTOTYPE(name, opcode, ops, src_modes, dst_modes)	\
	{#name, (opcode), (ops), {(src_modes), (dst_modes)}},

default_ins_prototype inst_prototypes[] = { INSTRUCTION_LIST(INS_MAKE_PROTOTYPE) /*,*/ {NULL, 0, 0, {0,0}} }; /* This line is not an error. */

default_ins_prototype* default_instruction_get_prototype(const char* name)
{
	default_ins_prototype *inst;

	/* Look up the symbol by name. */
	for_each_instruction(inst)
		if (strncmp(name, inst->name, strlen(inst->name)) == 0)
			return inst;

	return NULL;
}

default_instruction* default_instruction_make(const char *name)
{
	default_instruction *inst;

	default_ins_prototype *proto = default_instruction_get_prototype(name);

	if (proto == NULL)
		return NULL;

	if ((inst = malloc(sizeof(*inst))) == NULL)
		return NULL;

	inst->proto = proto;
	inst->type 	= 0;
	inst->dbl	= 0;
	inst->comb	= 0;

	list_init(&inst->insts);

	return inst;
}

#define is_valid_mode(mode) (!(mode & (mode - 1)) || mode > ADDRESSING_MAX_MODE)

void default_address_init(address *ad, enum addressing_mode mode, enum addressing_mode index_mode)
{
	if (ad == NULL || mode == NONE)
		return;

	/* Hack to test if mode is only one of the valid members of the enum. */
	if (!is_valid_mode(mode))
		return;

	if (mode == INDEX && !is_valid_mode(index_mode))
		return;

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
			ad->data.none				= 0;
			ad->index_data.symbol 		= NULL;
			ad->index_data.type = index_mode;
			switch (index_mode)
			{
				case IMMEDIATE:
					ad->index_data.index.immediate_data 	= 0;
					break;
				case DIRECT:
					ad->index_data.index.direct_sym 		= NULL;
					break;
				case REGISTER:
					ad->index_data.index.register_number	= 0;
					break;
				case INDEX:
				default:
					ad->index_data.index.none 			= 0;
					break;
			}
			break;
		case REGISTER:
			ad->data.register_number	= 0;
			break;
		default:
			ad->data.none				= 0;
	}
}

void default_instruction_destroy(default_instruction *inst)
{
	if (inst == NULL)
		return;

	list_remove(&inst->insts);

	free(inst);
}