#ifndef LIST_H
#define LIST_H

struct ListNode
{
	// Payload
	struct ListNode* next;
};

struct List
{
	struct ListNode* start;
	int counter;
};

void ListCreate(struct List**);

void ListInsert(struct List*,/*tupleToBeInserted*/);


void ListPrint(struct List*);

void ListDestroy(struct List*);


#endif