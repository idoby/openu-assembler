/* Intrusive implementation of circular doubly-linked list. */
#ifndef __INTRUSIVE_TREE_H
#define __INTRUSIVE_TREE_H

#include <container_of.h>

typedef struct tree_node {
	struct tree_node *left;
	struct tree_node *right;
} tree_node;

typedef struct
{
	tree_node *root_node;
} tree;

typedef void (*visit_func)(tree_node *element);
typedef int (*compare_func)(tree_node *e1, tree_node* e2);

#define tree_empty(tree) ((tree)->root_node == NULL)

void tree_init(tree *root);
void tree_node_init(tree_node* node);
void tree_insert(tree *root, tree_node *new, compare_func cmp);
tree_node* tree_search(tree *root, tree_node *find, compare_func cmp);
void tree_traverse(tree *root, visit_func visit);

#endif