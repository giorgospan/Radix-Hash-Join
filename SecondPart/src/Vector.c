#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>/*write()*/
#include <string.h>/*strlen()*/

#include "Vector.h"
#include "Utils.h"

unsigned initSize;

void createVector(struct Vector **vector,unsigned tupleSize)
{
	*vector              = allocate(sizeof(struct Vector),"createVector");
	(*vector)->table     = NULL;
	(*vector)->tupleSize = tupleSize;
	(*vector)->nextPos   = 0;
	(*vector)->capacity  = 0;
}

void createVectorFixedSize(struct Vector **vector,unsigned tupleSize,unsigned fixedSize)
{
	*vector              = allocate(sizeof(struct Vector),"createVectorFixedSize1");
	(*vector)->capacity  = tupleSize*fixedSize;
	(*vector)->tupleSize = tupleSize;
	(*vector)->table     = allocate((*vector)->capacity *sizeof(unsigned),"createVectorFixedSize2");
	(*vector)->nextPos   = 0;
}

void resetVector(struct Vector *vector,unsigned newTupleSize){
	vector->nextPos   = 0;
	vector->tupleSize = newTupleSize;
}

void insertAtVector(struct Vector *vector,unsigned *tuple)
{
	/* If vector is empty , create table */
	if(vectorIsEmpty(vector))
	{
		/* initSize is initialized according to avgNumOfTuples of the given relations */
		/* check Joiner.c */
		vector->capacity = initSize*vector->tupleSize;
		vector->table    = allocate(vector->capacity*sizeof(unsigned),"insertAtVector");
	}

	/* Else if vector is full, reallocate more space */
	else if(vectorIsFull(vector))
	{
		vector->capacity*=2;
		unsigned *new   = realloc(vector->table,vector->capacity*sizeof(unsigned));
		if(!new)
		{
			fprintf(stderr,"realloc failed[insertAtVector]\nExiting...\n");
			exit(EXIT_FAILURE);
		}
		vector->table    = new;
		// fprintf(stderr, "realloc() has been called\n");
	}

	/* Insert tuple */
	unsigned pos     	= vector->nextPos;
	for(unsigned i=0;i<vector->tupleSize;++i)
		vector->table[pos+i] = tuple[i];

	vector->nextPos+=vector->tupleSize;
}

/* Caution: This function will be called only for fixed size vectors */
/* Useful in case we want to insert a tuple in a specific offset inside the vector */
void insertAtPos(struct Vector *vector,unsigned *tuple,unsigned offset)
{
	unsigned pos = offset*vector->tupleSize;
	for(unsigned i=0;i<vector->tupleSize;++i)
		vector->table[pos+i] = tuple[i];
	vector->nextPos += vector->tupleSize;
}

void printVector(struct Vector *vector)
{
	if(vector->nextPos==0)
	{
		// fprintf(stderr,"Vector is empty\n");
		return;
	}
	/* nextPos holds the number of the rowIds */
	/* which is the same with the first available position in vector */
	unsigned k=0;
	for(unsigned i=0;i<vector->nextPos;++i)
		if(i%vector->tupleSize==0)
		{
			if(k++==10) // Stop after printing the first 10 tuples
				break;
			printTuple(vector,i);
		}
}

void printTuple(struct Vector *vector,unsigned pos)
{
	printf("(");
	for(unsigned i=0;i<vector->tupleSize;++i)
		if(i==vector->tupleSize-1)
			printf("%u",vector->table[pos+i]+1);
		else
			printf("%u,",vector->table[pos+i]+1);
	printf(")\n");
}

void scanColEquality(struct Vector *new,struct Vector* old,uint64_t *leftCol,uint64_t* rightCol,unsigned posLeft,unsigned posRight)
{
	/* Note: Each tuple in old vector contains one rowId */
	/* Except for the case of join between relations from the same intermediate entity[i.e: vector] */
	for(unsigned i=0;i<old->nextPos;i+=old->tupleSize)
		if(leftCol[old->table[i+posLeft]] == rightCol[old->table[i+posRight]])
			insertAtVector(new,&old->table[i]);
}

void scanFilter(struct Vector *new,struct Vector* old,uint64_t *col,Comparison cmp,uint64_t constant)
{
	/* Note: Each tuple in old vector contains one rowId */
	/* Insert every tuple of the old vector to the new one, if it satisfies the filter */
	for(unsigned i=0;i<old->nextPos;++i)
		if(compare(col[old->table[i]],cmp,constant))
			insertAtVector(new,&old->table[i]);
}

// Fill info with values and tuples
void scanJoin(RadixHashJoinInfo *joinRel)
{
	struct Vector *old    = joinRel->vector[0];
	struct Vector *new    = joinRel->unsorted->tuples;

	// Position of this relation's rowId inside tuple
	unsigned tupleOffset  = joinRel->map[joinRel->relId];

	unsigned sizeOfVector = old->nextPos;
	uint64_t *origValues  = joinRel->col;
	uint64_t *colValues   = joinRel->unsorted->values;
	unsigned *rowIds      = joinRel->unsorted->rowIds;

	unsigned k=0;
	// We scan the old vector
	for(unsigned i=0;i<sizeOfVector;i+=old->tupleSize)
	{
		unsigned origRowId = old->table[i+tupleOffset];
		// Add value
		colValues[k]       = origValues[origRowId];
		// Add tuple
		insertAtVector(new,&old->table[i]);
		// Add rowId
		rowIds[k++]        = k;
	}
}

void destroyVector(struct Vector **vector)
{
	if(*vector==NULL)
	{
		// fprintf(stderr,"Vector is empty\n");
		return;
	}
	free((*vector)->table);
	free(*vector);
	*vector = NULL;
}

int vectorIsFull(struct Vector *vector)
{
	return vector->nextPos == vector->capacity;
}

int vectorIsEmpty(struct Vector *vector)
{
	return vector->table == NULL;
}

unsigned getVectorTuples(struct Vector *vector)
{
	return vector->nextPos/vector->tupleSize;
}

/* Returns a pointer to the i-th tuple */
unsigned *getTuple(struct Vector *vector,unsigned i)
{
	if(i>vector->nextPos/vector->tupleSize)
	{
		fprintf(stderr, "Trying to access a tuple that does not exist\nExiting...\n\n");
		exit(EXIT_FAILURE);
	}
	return 	&vector->table[i*vector->tupleSize];
}

unsigned getTupleSize(struct Vector *vector)
{return vector->tupleSize;}


uint64_t checkSum(struct Vector *vector,uint64_t *col,unsigned rowIdPos)
{
	uint64_t sum=0;
	for(unsigned i=0;i<vector->nextPos;i+=vector->tupleSize)
		sum+=col[vector->table[i+rowIdPos]];
	return sum;
}
