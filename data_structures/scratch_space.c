#include "scratch_space.h"
#include <utils.h>

#include <string.h>

void scratch_init(scratch_space *s)
{
	if (s == NULL)
		return;

	/* Zero all the data. */
	memset(s->cells, 0, sizeof(s->cells));

	s->offset_next   = 0;
	s->offset_global = 0;
}

void scratch_rewind(scratch_space *s)
{
	if (s == NULL)
		return;

	s->offset_next = 0;
}

void scratch_write_cell(scratch_space *s, unsigned int data, mem_cell_type type)
{
	if (s == NULL)
		return;

	s->cells[s->offset_next].data = data;
	s->cells[s->offset_next].type = type;

	++s->offset_next;
}

unsigned int scratch_read_cell(scratch_space *s, unsigned int *data, mem_cell_type *type)
{
	if (s == NULL || data == NULL || type == NULL || s->offset_next >= MAX_MEM)
		return 0;

	*data = s->cells[s->offset_next].data;
	*type = s->cells[s->offset_next].type;

	++s->offset_next;

	return 1;
}

void scratch_set_global_offset(scratch_space *s, unsigned int offset)
{
	if (s == NULL)
		return;

	s->offset_global = offset;
}

unsigned int scratch_offset(scratch_space *s)
{
	return s == NULL ? 0 : s->offset_next;
}

unsigned int scratch_to_global(scratch_space *s, unsigned int position)
{
	return s == NULL ? 0 : position + s->offset_global; 
}