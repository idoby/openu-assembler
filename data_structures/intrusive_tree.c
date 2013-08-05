#include <stdlib.h>
#include "intrusive_tree.h"

void tree_init(tree *root)
{
	if (root == NULL)
		return;

	root->root_node = NULL;
}

void tree_node_init(tree_node* node)
{
	if (node == NULL)
		return;

	node->left	= NULL;
	node->right	= NULL;
}

void __tree_insert(tree_node *node, tree_node *new, compare_func cmp)
{
	int rel = cmp(node, new);
	
	/* If the nodes are equal, we can't insert, but the node is already there. */
	if (rel == 0)
		return;

	/* If the current node is bigger than the new one and there's a left subtree, go left. */
	if (rel == 1) {
		if (node->left != NULL)
			__tree_insert(node->left, new, cmp);
		else {
			node->left = new;		/* Found the right spot, insert new node here, coolness. */
			return;
		}
	}
	else if (rel == -1 && node->right != NULL)	/* Otherwise we go right and do something similar. */
		__tree_insert(node->right, new, cmp);
	else							/* Finally, if we need to go right but there's no subtree there. */
		node->right = new;
}

void tree_insert(tree *root, tree_node *new, compare_func cmp)
{
	if (root == NULL || new == NULL || cmp == NULL)
		return;

	/* Tree is empty, just place the new node as the root. */
	if (tree_empty(root))
	{
		root->root_node = new;
		return;
	}

	__tree_insert(root->root_node, new, cmp);
}

tree_node* __tree_search(tree_node *node, tree_node *find, compare_func cmp)
{
	int rel = cmp(node, find);

	/* If the nodes are equal, we found the droids we we're looking for. */
	if (rel == 0)
		return node;
	/* If the current node is bigger than the one we're looking for, go left. */
	else if (rel == 1) {
		if (node->left != NULL)
			__tree_search(node->left, find, cmp);
		else
			return NULL;
	}
	else if (rel == -1) {
		if (node->right != NULL)
			__tree_search(node->right, find, cmp);
		else
			return NULL;
	}
	
	/* Should not be reached if compare_func() follows the contract, but we need to shut the compiler up. */
	return NULL;
}

tree_node* tree_search(tree *root, tree_node *find, compare_func cmp)
{
	if (root == NULL || root->root_node == NULL || find == NULL || cmp == NULL)
		return NULL;

	return __tree_search(root->root_node, find, cmp);
}

/* Post-order traversal to allow this routine to be used for deletion of the tree. */
void __tree_traverse(tree_node *node, visit_func visit)
{
	/* If left child exists, recurse. */
	if (node->left != NULL)
		__tree_traverse(node->left, visit);

	/* Now the right. */
	if (node->right != NULL)
		__tree_traverse(node->right, visit);

	/* Visit the node itself first. */
	visit(node);
}

void tree_traverse(tree *root, visit_func visit)
{
	if (root == NULL || root->root_node == NULL || visit == NULL)
		return;

	__tree_traverse(root->root_node, visit);
}