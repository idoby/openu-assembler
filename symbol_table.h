
#ifndef __SYMBOL_TABLE_H
#define __SYMBOL_TABLE_H

#include "intrusive_list.h"
#include "intrusive_tree.h"

#define SYMBOL_MAX_LENGTH 30

typedef enum symbol_type {
	ENTRY,
	EXTERN,
	INTERN,
	INTERN_UNDEFINED
} symbol_type;

typedef tree_node table_element;
typedef tree symbol_table;

typedef struct symbol {
	char 					name[SYMBOL_MAX_LENGTH + 1];	/* +1 for null terminator, of course. */
	symbol_type				type;
	struct scratch_space	*address_space;			/* Pointer to where this symbol refers to. */
													/* Just a pointer, no need to include the actual declaration. */
	unsigned int			address_offset;			/* Offset within the address space. */
	list					orphaned_references;	/* List of references to the symbol when it's not defined yet. */
	table_element			sym_tree;				/* Binary search subtree for symbol table. */
} symbol;

#define table_entry(entry) container_of(symbol, entry, sym_tree)
typedef void (*table_visit_func)(table_element *symbol);

void	table_init(symbol_table* table);
void	table_new_symbol(symbol_table* table, const char* name, symbol_type type);
symbol*	table_find_symbol(symbol_table* table, const char* name);
void 	table_destroy(symbol_table *table);
void	table_traverse(symbol_table *table, table_visit_func visit);

#endif