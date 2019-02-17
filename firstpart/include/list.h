#ifndef LIST_H
#define LIST_H

#include "structDefinitions.h"

/* Buffer size in bytes [1 MB] */
// #define BUFFER_SIZE 1048576
#define BUFFER_SIZE 1024


struct ListNode
{
	// Payload
	struct resultTuple* buffer;
	struct ListNode* next;
};

struct List
{
	struct ListNode* first;
	struct ListNode* last;
	uint32_t nodeCounter;
	uint32_t tupleCounter;
	// We could add some more info later if we want.
	// We might not need the counters for now.
	// In case we dont need them later , we'll remove them.
};

void ListCreate(struct List**);

void ListCreateNode(struct ListNode** new,struct resultTuple* tuple);

void ListInsert(struct List* list,struct resultTuple* tuple);

void ListPrint(struct List*);

void ListDestroy(struct List*);

uint32_t isFull(struct resultTuple* buffer);

#endif