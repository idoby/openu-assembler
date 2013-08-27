#include "default_instruction.h"

#include <string.h>
#include <stdlib.h>

#define INSTRUCTION_LIST(list_entry) \
	           /*name  opcode  #operands 1st allowed modes 2nd allowed modes.*/ \
	list_entry (mov,   00,     2,        ALL,              NO_IMMEDIATE) \
	list_entry (cmp,   01,     2,        ALL,              ALL         ) \
	list_entry (add,   02,     2,        ALL,              NO_IMMEDIATE) \
	list_entry (sub,   03,     2,        ALL,              NO_IMMEDIATE) \
	list_entry (not,   04,     1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (clr,   05,     1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (lea,   06,     2,        NO_IMMEDIATE,     NO_IMMEDIATE) \
	list_entry (inc,   07,     1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (dec,   010,    1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (jmp,   011,    1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (bne,   012,    1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (red,   013,    1,        NO_IMMEDIATE,     NONE        ) \
	list_entry (prn,   014,    1,        ALL,              NONE        ) \
	list_entry (jsr,   015,    1,        IMMEDIATE,        NONE        ) \
	list_entry (rts,   016,    0,        NONE,	       NONE        ) \
	list_entry (stop,  017,    0,        NONE,	       NONE        )

#define INS_MAKE_PROTOTYPE(name, opcode, ops, src_modes, dst_modes) \
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
	inst->type  = 0;
	inst->dbl   = 0;
	inst->comb  = 0;

	list_init(&inst->insts);

	return inst;
}

#define is_valid_mode(mode) (!(mode & (mode - 1)) || mode > ADDRESSING_MAX_MODE)

void default_address_set_immediate(address *ad, int immediate)
{
	if (ad == NULL)
		return;

	ad->type                = IMMEDIATE;
	ad->data.immediate_data = immediate;
}

int default_address_get_immediate(address *ad)
{
	if (ad == NULL)
		return 0;

	return ad->data.immediate_data;
}

void default_address_set_register(address *ad, int reg_num)
{
	if (ad == NULL)
		return;

	ad->type                 = REGISTER;
	ad->data.register_number = reg_num;
}

int default_address_get_register(address *ad)
{
	if (ad == NULL)
		return INVALID_REGISTER;

	return ad->data.register_number;
}

void default_address_set_symbol(address *ad, symbol *sym)
{
	if (ad == NULL || sym == NULL)
		return;

	ad->type     = DIRECT;
	ad->data.sym = sym;
}

symbol* default_address_get_symbol(address *ad)
{
	if (ad == NULL)
		return NULL;

	return ad->data.sym;
}

void default_address_set_index_number(address *ad, int num)
{
	if (ad == NULL)
		return;

	ad->type = INDEX;

	ad->index_type                = IMMEDIATE;
	ad->index_data.immediate_data = num;
}

void default_address_set_index_register(address *ad, int reg_num)
{
	if (ad == NULL)
		return;

	ad->type = INDEX;

	ad->index_type                 = REGISTER;
	ad->index_data.register_number = reg_num;
}

void default_address_set_index_symbol(address *ad, symbol *sym)
{
	if (ad == NULL)
		return;

	ad->type = INDEX;

	ad->index_type     = DIRECT;
	ad->index_data.sym = sym;
}

void default_instruction_destroy(default_instruction *inst)
{
	if (inst == NULL)
		return;

	list_remove(&inst->insts);

	free(inst);
}