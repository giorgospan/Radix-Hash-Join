#include "usefulHeaders.h"

#include "list.h"

/*  First available place inside buffer */
static uint32_t available=0;
/* Number of buffer entries */
static uint32_t bufferEntries;

void ListCreate(struct List** list)
{
	
	if( (*list = malloc(sizeof(struct List)))==NULL)
	{
		perror("ListCreate()");
		exit(1);
	}
	
	(*list) -> first = NULL;
	(*list) -> last = NULL;
	(*list) -> nodeCounter = 0;
	(*list) -> tupleCounter = 0;
	bufferEntries = BUFFER_SIZE / sizeof(struct resultTuple);
}

/* This function might be called in the following cases: 
 	1. List is empty, thus we need to create our first node.
 	2. List is not empty, but the buffer in the last node is full. */
void ListCreateNode(struct ListNode** new,struct resultTuple* tuple)
{

	/* Allocate a new node */
	if( (*new = malloc(sizeof(struct ListNode)))==NULL)
	{
		perror("ListInsert()");
		exit(1);
	}

	/* Allocate node's buffer */
	if( ((*new)->buffer = malloc(bufferEntries*sizeof(struct resultTuple)))==NULL)
	{
		perror("ListInsert()");
		exit(1);
	}

	/* Add tuple to the first place of the buffer */
	(*new)->buffer[0].rowId1 = tuple->rowId1;
	(*new)->buffer[0].rowId2 = tuple->rowId2;
	available=1;

	/* Next node is null */
	(*new)->next = NULL;
}


void ListInsert(struct List* list,struct resultTuple* tuple)
{

	/* List is empty [i.e: this is our first insert] */
	if(list->first==NULL)
	{
		/* Create a new node */
		ListCreateNode(&list->first,tuple);

		/* Increment counters */
		list->nodeCounter++;
		list->tupleCounter++;

		/* Now first and last point both to the newly created node */
		list->last = list->first;
	}

	/* List is not empty, but the buffer in the last node is full */
	else if(isFull(list->last->buffer))
	{
		/* Create a new node */
		struct ListNode *new;
		ListCreateNode(&new,tuple);

		/* Increment counters */
		list->nodeCounter++;
		list->tupleCounter++;

		/* New node is now the last one */
		list->last->next = new;
		list->last = new;
	}

	/* Buffer in the last node is not full */
	else
	{
		/* Add tuple to the first available place inside the buffer */
		list->last->buffer[available].rowId1 = tuple->rowId1;
		list->last->buffer[available].rowId2 = tuple->rowId2;
		available++;

		/* Increment tuple counter */
		list->tupleCounter++;
	}
}

void ListPrint(struct List* list)
{
	struct ListNode* current = list->first;

	// Replace fp with stdout in case you want to print output on command line 

	FILE* fp = fopen("results.log","w");

	fprintf(fp,"List Nodes: %u | List Tuples: %u\n[Buffer size: %.2f KB]\n",list->nodeCounter,list->tupleCounter,BUFFER_SIZE/1000.0);
	if(!list->nodeCounter)
	{
		fprintf(fp,"List is empty\n");
		return;
	}

	/* Print buffer of each node */
	uint32_t i;	
	while(current)
	{
		/* Buffer in the last node might not be full */
		/* Thus, we iterate until the first empty[i.e: available] space */
		if(current==list->last)
			for(i=0;i<available;i++)
				fprintf(fp,"(%u,%u)\n",current->buffer[i].rowId1,current->buffer[i].rowId2);
		else
			for(i=0;i<bufferEntries;i++)
				fprintf(fp,"(%u,%u)\n",current->buffer[i].rowId1,current->buffer[i].rowId2);

		fprintf(fp,"=======================================================\n");
		current=current->next;
	}
	fclose(fp);
}


void ListDestroy(struct List* list)
{
	
	struct ListNode* current;
	struct ListNode* temp;
	current = list->first;
	
	/*Loop until we've reached end of list*/
	while(current)
	{
		/* Free buffer in current node */
		free(current->buffer);

		temp = current;
		current = current -> next;

		/* Free the node itself */
		free(temp);
	}
	
	/* Free the main list node */
	free(list);
}

uint32_t isFull(struct resultTuple* buffer)
{
	/* Next available place inside the buffer
		is bufferEntries + 1 [i.e: outside the buffer] 
		So, buffer is full. */
	return available >= bufferEntries;
}