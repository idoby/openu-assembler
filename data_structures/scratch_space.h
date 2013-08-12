#ifndef __SCRATCH_SPACE_H
#define __SCRATCH_SPACE_H
#include <utils.h>

/* Size of memory space. */
#define MAX_MEM 2000
/* Bits per machine word. */
#define WORD_SIZE_BITS 20

typedef enum mem_cell_type {
	ABSOLUTE = 0,
	EXTERNAL,
	RELOCATABLE,
	CELL_TYPE_MAX = RELOCATABLE
} mem_cell_type;

typedef struct scratch_space {
	struct {
		unsigned int data : WORD_SIZE_BITS;		/*	A cell of memory consists of the data stored */
		unsigned int type : LOG2(CELL_TYPE_MAX);/*	and the type of the cell, to be printed out later. */
	} cells[MAX_MEM];

	unsigned int offset_next;					/* DC/IC, offset of the next word to be written to the buffer. */
	unsigned int offset_global;					/* Offset from which the buffer is dumped to memory. */
} scratch_space;

void		scratch_init(scratch_space *s);
void		scratch_rewind(scratch_space *s);
unsigned int scratch_read_next_data(scratch_space *s, unsigned int *data, mem_cell_type *type);
void		scratch_write_next_data(scratch_space *s, unsigned int data, mem_cell_type type);
void		scratch_set_global_offset(scratch_space *s, unsigned int offset);
unsigned int scratch_get_next_offset(scratch_space *s);
unsigned int scratch_get_global_offset(scratch_space *s, unsigned int position);

#endif