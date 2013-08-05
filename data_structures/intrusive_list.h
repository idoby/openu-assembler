/* Intrusive implementation of circular doubly-linked list. */
#ifndef __INTRUSIVE_LIST_H
#define __INTRUSIVE_LIST_H

#include <container_of.h>

typedef struct list {
	struct list *next;
	struct list *prev;
} list;

/* Macro for iterating over the list, requires the use of container_of in the body. */
#define list_for_each(head, current) \
	for (current = (head).next; current != &(head); current = current->next)

/* Traverse the list, automatically applying container_of. */
#define list_for_each_entry(head, current, type, member) \
	for (	current = container_of(type, (head)->next, member);	\
			&(current->member) != (head);						\
			current = container_of(type, current->member.next, member))

/* Same as above, deletion-proof version. List elements can be removed while iterating. */
#define list_for_each_entry_safe(head, current, safe, type, member) \
	for (	current = container_of(type, (head)->next, member),		\
			safe = container_of(type, current->member.next, member);\
			&(current->member) != (head);							\
			current = safe, safe = container_of(type, safe->member.next, member))

#define list_empty(head) ((head)->next == (head))

void list_init(list *head);
void list_insert_after(list *head, list *new);
void list_insert_before(list *head, list *new);
void list_remove(list *element);

#endif