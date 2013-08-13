typedef struct default_instruction_encoding {
	unsigned int comb			: 2;

	unsigned int dst_reg_num	: 3;
	unsigned int dst_add_type	: 2;

	unsigned int src_reg_num	: 3;
	unsigned int src_add_type	: 2;

	unsigned int opcode 		: 4;
	unsigned int type			: 1;
	unsigned int dbl 			: 1;

	unsigned int unused			: 32 - 18;
} default_instruction_encoding;

enum addressing_codes {
	IMMEDIATE_CODE	= 0,
	DIRECT_CODE		= 1,
	INDEX_CODE		= 2,
	REGISTER_CODE	= 3
};

#define zero_addressing_mode(where)	do { 	\
		enc->where ## _add_type = 0;		\
		enc->where ## _reg_num  = 0;		\
		} while(0);

#define map_addressing_mode_type(ad, where)	do { mode_switch(ad)	\
											{					\
												map_code(IMMEDIATE, where)	\
												map_code(DIRECT, where)		\
												map_code(INDEX, where)		\
												map_code(REGISTER, where)	\
												map_code_else(where)		\
											} } while(0)

#define map_addressing_mode_reg(ad, where)	\
		do { if ((ad).type == REGISTER)	\
			enc->where ## _reg_num = (ad).data.register_number;	\
		else if ((ad).type == INDEX && (ad).index_type == REGISTER)	\
			enc->where ## _reg_num = (ad).index_data.register_number;	\
		else	\
			enc->where ## _reg_num = 0; } while(0)

#define mode_switch(ad) switch((ad).type)

#define map_code(mode, where)	\
	case mode:		\
		enc->where ## _add_type = mode ## _CODE;	\
		break;

#define map_code_else(where)	\
	default:			\
		enc->where ## _add_type = 0;

static int __finalize_check_labels(table_element *elem, void *arg)
{
	symbol *sym = table_entry(elem);
	list *err_list = arg;

	if (!table_is_defined(sym))
	{
		error *err = error_make(ERROR_NO_LINE, "Label '%s' is undefined!", sym->name);
		list_insert_before(err_list, &err->errors);
		return 0;
	}

	return 1;
}

static void __finalize_encode(default_instruction *inst, default_instruction_encoding *enc)
{
	enc->unused		= 0;
	enc->dbl		= inst->dbl;
	enc->type		= inst->type;
	enc->opcode		= inst->proto->opcode;
	enc->comb		= inst->comb;

	if (inst->proto->num_operands == MAX_OPERANDS)
	{
		map_addressing_mode_type(inst->operands[0], src);
		map_addressing_mode_reg(inst->operands[0], src);

		map_addressing_mode_type(inst->operands[1], dst);
		map_addressing_mode_reg(inst->operands[1], dst);
	}
	else if (inst->proto->num_operands == 1)
	{
		zero_addressing_mode(src);

		map_addressing_mode_type(inst->operands[0], dst);
		map_addressing_mode_reg(inst->operands[0], dst);
	}
	else
	{
		zero_addressing_mode(src);
		zero_addressing_mode(dst);
	}
}

static void __finalize_write_direct(scratch_space *s, symbol *sym)
{
	unsigned int global_off;
	if (table_is_extern(sym))
		scratch_write_next_data(s,	0, EXTERNAL);
	else
	{
		global_off = scratch_get_global_offset(	sym->address_space,
												sym->address_offset);
		scratch_write_next_data(s, global_off, RELOCATABLE);
	}
}

static void __finalize_translate_instruction(default_translate_context *dtc, default_instruction *inst)
{
	unsigned int operand = 0;
	default_instruction_encoding enc;
	scratch_space *is = dtc->i_scratch;
	unsigned int inst_off = scratch_get_global_offset(is, inst->address_offset);

	/* Encode and write the word that represents the instruction. */
	__finalize_encode(inst, &enc);
	scratch_write_next_data(is, *(unsigned int*)(&enc), ABSOLUTE);

	for (; operand < inst->proto->num_operands; ++operand)
	{
		address ad = inst->operands[operand];
		int index_distance;
		symbol *sym;

		switch (ad.type)
		{
			case IMMEDIATE:
				/* Write the value of the immediate operand to the next memory word. */
				scratch_write_next_data(is,	ad.data.immediate_data,	ABSOLUTE);
				break;
			case DIRECT:
				/* Write the global value of the symbol to the next memory word. */
				__finalize_write_direct(is, ad.data.sym);
				break;
			case INDEX:
				__finalize_write_direct(is, ad.data.sym);

				switch(ad.index_type)
				{
					case IMMEDIATE:
						scratch_write_next_data(is, ad.index_data.immediate_data, ABSOLUTE);
						break;
					case DIRECT:
						/* Calculate the distance to the target symbol and write it to another word. */
						sym = ad.index_data.sym;
						index_distance = scratch_get_global_offset(sym->address_space, sym->address_offset) - inst_off;

						scratch_write_next_data(is, index_distance, ABSOLUTE);
						break;
					default:
						break; /* Impossible at this stage. */
				}

				break;
			default:
				break; /* Impossible at this stage. */
		}
	}
}