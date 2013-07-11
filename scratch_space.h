#ifndef __SCRATCH_SPACE_H
#define __SCRATCH_SPACE_H
#include "utils.h"

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

struct scratch_space {
	struct {									/*	A cell of memory consists of the data stored */
		unsigned int data : WORD_SIZE_BITS;		/*	and the type of the cell, to be printed out later. */
		unsigned int type : LOG2(CELL_TYPE_MAX);
	} cells[MAX_MEM];

	unsigned int offset_next;					/* DC/IC, offset of the next word to be written to the buffer. */
	unsigned int offset_global;					/* Offset from which the buffer is dumped to memory. */
};

#endif