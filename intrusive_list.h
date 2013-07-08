/* Intrusive implementation of circular doubly-linked list. */
#ifndef __INTRUSIVE_LIST_H
#define __INTRUSIVE_LIST_H

#include "container_of.h"

typedef struct list {
	struct list *next;
	struct list *prev;
} list;

#define list_for_each(head, current) \
	for (current = (head).next; current != &(head); current = current->next)

#define list_for_each_entry(head, current, entry, type, member) \
	for (current = (head).next; entry = container_of(type, current, member), current != &(head); current = current->next)

#define list_empty(head) ((head)->next == (head))

void list_init(list *head);
void list_insert_after(list *head, list *new);
void list_insert_before(list *head, list *new);
void list_remove(list *element);

#endif