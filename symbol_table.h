
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

typedef struct symbol {
	char name[SYMBOL_MAX_LENGTH + 1]; /* +1 for null terminator, of course. */
	symbol_type type;
	list orphaned_references;
	tree_node sym_tree;
} symbol;

typedef tree symbol_table;

void	table_init(symbol_table* table);
void	table_new_symbol(symbol_table* table, const char* name, symbol_type type);
symbol*	table_find_symbol(symbol_table* table, const char* name);
void 	table_destroy(symbol_table* table);
int 	table_compare_symbols(tree_node *e1, tree_node* e2);

#endif