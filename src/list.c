#include "usefulHeaders.h"

#include "list.h"

void ListCreate(struct List** list)
{
	
	if( (*list = malloc(sizeof(struct List)))==NULL)
	{
		perror("ListCreate()");
		exit(1);
	}
	
	(*list) -> start = NULL;
	(*list) -> counter = 0;
}


void ListInsert(struct List* list,char* payload,int type)
{
	struct ListNode* new;
	
	/*Create a new node*/
	if( (new = malloc(sizeof(struct ListNode)))==NULL)
	{
		perror("ListInsert()");
		exit(1);
	}

	/* Add payload to the newly created node */
	
	/*Attach the new node to the list*/
	new->next = list->start;
	list->start = new;
	
	/*Increase number of elements*/
	list->counter ++ ;
}

void ListPrint(struct List* list)
{
	struct ListNode* current = list->start;
	if(!list->counter)printf("List is empty\n");
	
	while(current)
	{
		/* Print buffer in current node */
		current=current->next;
	}
}


void ListDestroy(struct List* list)
{
	
	struct ListNode* current;
	struct ListNode* temp;
	current = list->start;
	
	/*Loop until we've reached end of list*/
	while(current)
	{
		/* Free buffer in current node */

		temp = current;
		current = current -> next;

		/* Free the node itself */
		free(temp);
	}
	
	/* Free the main list node */
	free(list);
}