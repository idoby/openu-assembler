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

/* Function prototype for tree traversal user functions. This function is
   called for every node in the tree and takes one void* argument that is
   passed to the tree_traverse. */
typedef int (*visit_func)(tree_node *element, void *arg);
/* Function prototype used to compare two tree nodes to maintain order in the tree.
   Should return 0, 1 or -1. */
typedef int (*compare_func)(tree_node *e1, tree_node* e2);

/* Predicate to decide if the tree is empty. */
#define tree_empty(tree) ((tree)->root_node == NULL)

/* Initializes a tree object. */
void tree_init(tree *root);
/* Initializes a tree_node object. */
void tree_node_init(tree_node* node);
/* Inserts new into the tree pointed to by root using cmp as an order predicate function. */
void tree_insert(tree *root, tree_node *new, compare_func cmp);
/* Searches for find in the tree pointed to by root using cmp. */
tree_node* tree_search(tree *root, tree_node *find, compare_func cmp);
/* Calls visit with arg for each node in the tree pointed to by root.
   This is a post-order traversal. */
int tree_traverse(tree *root, visit_func visit, void *arg);

#endif