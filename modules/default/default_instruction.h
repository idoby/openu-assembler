#ifndef __DEFAULT_INSTRUCTION_H
#define __DEFAULT_INSTRUCTION_H

#include <data_structures/intrusive_list.h>
#include <data_structures/symbol_table.h>
#include <data_structures/scratch_space.h>

#define MAX_OPERANDS				2
#define INSTRUCTION_NAME_MAX_LEN 	4
#define INVALID_REGISTER			-1

typedef union address_data {
	int immediate_data;
	symbol *sym;
	int register_number;
	int none;
} address_data;

typedef enum addressing_mode {
	IMMEDIATE 	= 1,
	DIRECT		= 2,
	INDEX		= 4,
	REGISTER	= 8,
	ALL = IMMEDIATE | DIRECT | INDEX | REGISTER,
	NO_IMMEDIATE = DIRECT | INDEX | REGISTER,
	NONE = 0,
	ADDRESSING_MAX_MODE = REGISTER
} addressing_mode;

typedef struct address {
	addressing_mode type;
	address_data data;

	addressing_mode index_type;
	address_data index_data;
} address;

typedef struct default_ins_prototype {
	char *name;
	unsigned int opcode;
	unsigned int num_operands;
	enum addressing_mode allowed_modes[MAX_OPERANDS];
} default_ins_prototype;

/* A type representing one parsed instruction. */
typedef struct default_instruction {
	default_ins_prototype *proto;
	unsigned int type;
	unsigned int dbl;
	unsigned int comb;
	scratch_space *address_space;
	unsigned int address_offset;
	address operands[MAX_OPERANDS];	/* First is destination, second is source. */
	list	insts;
	unsigned int src_line;					/* Source line number in the input file. */
} default_instruction;

#define for_each_instruction(inst) for ((inst) = inst_prototypes; (inst)->name != NULL; ++inst)

/* Exported out of the module for testing, but still should not be used independently! */

/* Returns a new instruction object with the prototype already set, based on name. */
default_instruction*	default_instruction_make(const char *name);
/* Destroys an instruction object given by make_instruction. */
void					default_instruction_destroy(default_instruction *inst);
/* Gets an instruction prototype from name. */
default_ins_prototype*	default_instruction_get_prototype(const char* name);

void default_address_init(address *ad, enum addressing_mode mode, enum addressing_mode index_mode);

void default_address_set_immediate(address *ad, int immediate);
int default_address_get_immediate(address *ad);

void default_address_set_register(address *ad, int reg_num);
int default_address_get_register(address *ad);

void default_address_set_symbol(address *ad, symbol *sym);
symbol* default_address_get_symbol(address *ad);

void default_address_set_index_number(address *ad, int num);

void default_address_set_index_register(address *ad, int reg_num);

void default_address_set_index_symbol(address *ad, symbol *sym);

#endif