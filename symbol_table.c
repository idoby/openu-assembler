#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define table_insert(table, new) tree_insert(table, new, __table_compare_symbols)

/* Delete a single element from the tree. */
static void __delete_element(tree_node *element)
{
	symbol *sym = table_entry(element);

	/* TODO: delete orphaned reference list. */

	free(sym);
}

void table_init(symbol_table* table)
{
	tree_init(table);
}

static int __table_compare_symbols(tree_node *e1, tree_node* e2)
{
	symbol *sym1 = table_entry(e1);
	symbol *sym2 = table_entry(e2);
	int rel;

	if (sym1->name == NULL || sym2->name == NULL)
		return 0;

	rel = strncmp(sym1->name, sym2->name, SYMBOL_MAX_LENGTH + 1);

	if (rel > 0)
		return 1;
	else if (rel < 0)
		return -1;

	return 0;
}

void table_new_symbol(symbol_table* table, const char* name, symbol_type type)
{
	symbol *sym = malloc(sizeof(symbol));

	if (sym == NULL)
		return;

	/* Initialize the new symbol object. */
	sym->type = type;
	sym->address_space = NULL;
	sym->address_offset = 0;
	strncpy(sym->name, name, SYMBOL_MAX_LENGTH + 1);
	sym->name[SYMBOL_MAX_LENGTH] = '\0';
	list_init(&sym->orphaned_references);
	tree_node_init(&sym->sym_tree);

	/* Insert into table. */
	table_insert(table, &sym->sym_tree);
}

symbol*	table_find_symbol(symbol_table* table, const char* name)
{
	tree_node *res;
	/*	Create a new temporary symbol to be able to search for it using the tree function.
		No need to initialize properly because we only care about the name for the comparison. */
	symbol sym;
	strncpy(sym.name, name, SYMBOL_MAX_LENGTH + 1); /* Copy the name to the temp symbol. */
	sym.name[SYMBOL_MAX_LENGTH] = '\0';

	res = tree_search(table, &sym.sym_tree, __table_compare_symbols);

	if (res == NULL)
		return NULL;

	return table_entry(res);
}

void table_destroy(symbol_table* table)
{
	tree_traverse(table, __delete_element);
	table->root_node = NULL;
}

void table_traverse(symbol_table *table, table_visit_func visit)
{
	tree_traverse(table, visit);
}