
#ifndef __SYMBOL_TABLE_H
#define __SYMBOL_TABLE_H

#include "intrusive_list.h"
#include "intrusive_tree.h"

#define SYMBOL_MAX_LENGTH 30

typedef enum symbol_type {
	ENTRY,
	EXTERN,
	INTERN
} symbol_type;

typedef tree_node table_element;
typedef tree symbol_table;

typedef struct symbol {
	char 					name[SYMBOL_MAX_LENGTH + 1];	/* +1 for null terminator, of course. */
	symbol_type				type;
	unsigned int			defined;
	struct scratch_space	*address_space;			/* Pointer to where this symbol refers to. */
													/* Just a pointer, no need to include the actual declaration. */
	unsigned int			address_offset;			/* Offset within the address space. */
	list					references;	/* List of references to the symbol. */
	table_element			sym_tree;				/* Binary search subtree for symbol table. */
} symbol;

		/*	Converts from a list* to its containing symbol*. */
#define table_entry(entry) container_of(symbol, entry, sym_tree)

		/*	Prototype for function handling each symbol in the table in traversal. */
typedef void (*table_visit_func)(table_element *symbol);
		/*	Prototype for function handling each instruction in a reference list in traversal. */
typedef void (*table_consume_func)(void *inst);

		/*	Table initializer. Must be called after allocation. */
void	table_init(symbol_table* table);

		/*	Insert a new symbol into the table. */
symbol*	table_new_symbol(symbol_table* table, const char* name);

		/*	Look up a symbol in the table by name.
			Returns a pointer to the new symbol. */
symbol*	table_find_symbol(symbol_table* table, const char* name);

		/* Sets the address space and offset for a symbol. */
void	table_set_address_space(symbol *sym, struct scratch_space *s, unsigned int offset);

		/* Sets the type for a symbol. */
void	table_set_type(symbol *sym, symbol_type type);

		/* Sets a symbol to be defined. */
void	table_set_defined(symbol *sym);

		/* Returns 1 if symbol is defined. */
int		table_is_defined(symbol *sym);

		/* Returns 1 if symbol is an entry. */
int		table_is_entry(symbol *sym);

		/*	Table destructor. Frees up all the memory. Remember to call! */
void 	table_destroy(symbol_table *table);

		/*	Iterates over all symbols in the table. */
void	table_traverse(symbol_table *table, table_visit_func visit);

		/*	Add a new orphaned reference to a symbol.
			Returns 1 on success or 0 on error. This is because I don't
			want to expose the implementation of references to the client. */
int		table_add_reference(symbol *sym, void *inst);

		/*	Iterate over the references to a symbol, deleting them as we go. */
void	table_consume_references(symbol *sym, table_consume_func consume);

#endif