#include <stdlib.h>
#include "intrusive_list.h"

void list_init(list *head)
{
	if (head == NULL)
		return;

	head->next = head;
	head->prev = head;
}

void list_insert_after(list *head, list *new)
{
	if (head == NULL || new == NULL)
		return;

	head->next->prev = new;
	new->prev        = head;
	new->next        = head->next;
	head->next       = new;
}

void list_insert_before(list *head, list *new)
{
	list_insert_after(head->prev, new);
}

void list_remove(list *element)
{
	if (element == NULL)
		return;

	element->prev->next = element->next;
	element->next->prev = element->prev;
}