#ifndef __DEFAULT_INSTRUCTION_H
#define __DEFAULT_INSTRUCTION_H

#include <data_structures/intrusive_list.h>
#include <data_structures/symbol_table.h>
#include <data_structures/scratch_space.h>

#define MAX_OPERANDS				2
#define INSTRUCTION_NAME_MAX_LEN 	4

union address_data {
	int immediate_data;
	symbol *direct_sym;
	unsigned int register_number;
	int none;
};

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

typedef struct address {
	enum addressing_mode type;
	union address_data data;
	struct index_data
	{
		struct symbol *symbol;
		enum addressing_mode type;
		union address_data index;
	} index_data;
} address;

typedef struct default_ins_prototype {
	char *name;
	unsigned int opcode;
	unsigned int num_operands;
	unsigned int allowed_modes[MAX_OPERANDS];
} default_ins_prototype;

/* A type representing one parsed instruction. */
typedef struct default_instruction {
	default_ins_prototype *proto;
	unsigned int type;
	unsigned int dbl;
	unsigned int comb;
	scratch_space *address_space;
	unsigned int address_offset;
	struct address operands[MAX_OPERANDS];	/* First is destination, second is source. */
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

#endif