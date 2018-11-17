#ifndef DEFINITIONS_H
#define DEFINITIONS_H 

#include "usefulHeaders.h"

struct placeHolder
{
	uint64_t numOfRows;
	uint64_t numOfColumns;
	/* This points to the start of every column */
	uint64_t **arrayPointer;
};


/* Creates the array of size equal to the total number of relations */
struct placeHolder* initRelationArray(uint64_t numOfRelations);

/* Free the memory Be careful there is another pointer in the struct */
void freeRelationArray(struct placeHolder *array, uint64_t numOfRelations);

/* Just print the struct fields */
void printPlaceholder(struct placeHolder* st);

#endif