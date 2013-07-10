
#ifndef __SYMBOL_TABLE_H
#define __SYMBOL_TABLE_H

#include "intrusive_list.h"
#include "intrusive_tree.h"
#include "translate.h"

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
typedef void (*table_consume_func)(instruction *inst);

		/* Table initializer. Must be called after allocation. */
void	table_init(symbol_table* table);
		/* Insert a new symbol into the table. */
void	table_new_symbol(symbol_table* table, const char* name, symbol_type type);
		/* Look up a symbol in the table by name. */
symbol*	table_find_symbol(symbol_table* table, const char* name);
		/* Table destructor. Frees up all the memory. Remember to call! */
void 	table_destroy(symbol_table *table);
		/* Iterates over all symbols in the table. */
void	table_traverse(symbol_table *table, table_visit_func visit);

		/* Add a new orphaned reference to a symbol. */
void	table_add_reference(symbol *sym, instruction *inst);
		/* Iterate over the references to a symbol, deleting them as we go. */
void	table_consume_references(symbol *sym, table_consume_func consume);

#endif