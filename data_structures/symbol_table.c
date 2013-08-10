#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"

#define table_insert(table, new) tree_insert(table, new, __table_compare_symbols)

struct reference {
	void *inst;
	list refs;
};

/* Delete a single element from the tree. */
static void __delete_element(table_element *element)
{
	struct reference *ref, *safe;
	symbol *sym = table_entry(element);

	/* Delete references, if any exist. */
	list_for_each_entry_safe(	&sym->references,
								ref,
								safe,
								struct reference,
								refs)
	{
		list_remove(&ref->refs);
		free(ref);
	}

	/* Deallocate symbol itself. */
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

symbol* table_new_symbol(symbol_table* table, const char* name)
{
	symbol *sym;

	if ((sym = malloc(sizeof(*sym))) == NULL)
		return NULL;

	/* Initialize the new symbol object. */
	sym->type			= INTERN;
	sym->defined		= 0;
	sym->address_space	= NULL;
	sym->address_offset	= 0;
	strncpy(sym->name, name, SYMBOL_MAX_LENGTH + 1);
	sym->name[SYMBOL_MAX_LENGTH] = '\0';
	list_init(&sym->references);
	tree_node_init(&sym->sym_tree);

	/* Insert into table. */
	table_insert(table, &sym->sym_tree);

	return sym;
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

int table_add_reference(symbol *sym, void *inst)
{
	struct reference *ref;

	if (sym == NULL || inst == NULL)
		return 0;

	if ((ref = malloc(sizeof(*ref))) == NULL)
		return 0;

	ref->inst = inst;

	/* Insert new reference object at the end of the list. */
	list_insert_before(&sym->references, &ref->refs);

	return 1;
}

void table_consume_references(symbol *sym, table_consume_func consume)
{
	struct reference *ref, *safe;

	if (sym == NULL || consume == NULL)
		return;

	list_for_each_entry_safe(&sym->references, ref, safe, struct reference, refs)
	{
		consume(ref->inst);

		list_remove(&ref->refs);
		free(ref);
	}
}

void table_set_address_space(symbol *sym, struct scratch_space *s, unsigned int offset)
{
	if (sym == NULL || s == NULL)
		return;

	sym->address_space	= s;
	sym->address_offset	= offset;
}

void table_set_type(symbol *sym, symbol_type type)
{
	if (sym == NULL)
		return;

	sym->type = type;
}

void table_set_defined(symbol *sym)
{
	if (sym == NULL)
		return;

	sym->defined = 1;
}

int table_is_defined(symbol *sym)
{
	return sym == NULL ? 0 : sym->defined;
}

int table_is_entry(symbol *sym)
{
	return sym == NULL ? 0 : (sym->type == ENTRY);
}

int table_is_extern(symbol *sym)
{
	return sym == NULL ? 0 : (sym->type == EXTERN);
}