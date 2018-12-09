#ifndef VECTOR_H
#define VECTOR_H
#include <stdint.h>

#include "Utils.h" /*Comparison type*/
#include "Intermediate.h" /*JoinArg type*/
#include "Partition.h"/*JoinColumn type*/

struct Vector
{
	/* Table with tuples */
	unsigned *table;

	/* Size of tuple (i.e: rowids per tuple) */
	unsigned tupleSize;
	
	/* Position where the next tuple will be inserted 
	 * This member also acts as a counter for the vector elements
	 * i.e:for the rowIds stored in the vector 
	 */
	unsigned nextPos;

	/* Max capacity[i.e: number of rowIds] of the table */
	/* If needed, we'll double it using realloc(..) */
	unsigned capacity;
};

/* Creators/Destroyer */
void createVector(struct Vector **vector,unsigned tupleSize);
/**
 * @brief      Creates a vector fixed size.
 *
 * @param      vector     The vector
 * @param[in]  tupleSize  The tuple size
 * @param[in]  fixedSize  The number of tuples that will be stored in it
 */
void createVectorFixedSize(struct Vector **vector,unsigned tupleSize,unsigned fixedSize);
void destroyVector(struct Vector **vector);

/* Insert functions. No reason for delete function.*/
void insertAtVector(struct Vector *vector,unsigned *tuple);
void insertAtPos(struct Vector *vector,unsigned *tuple,unsigned offset);


/* Getter functions */
unsigned getVectorTuples(struct Vector *vector);
unsigned getTupleSize(struct Vector *vector);
unsigned *getTuple(struct Vector *vector,unsigned i);

/* Scan functions [Used in the case of an intermediate relation]*/
void scanColEquality(struct Vector *new,struct Vector* old,uint64_t *leftCol,uint64_t* rightCol,unsigned posLeft,unsigned posRight);
void scanFilter(struct Vector *new,struct Vector* old,uint64_t *col,Comparison cmp,uint64_t constant);
void scanJoin(RadixHashJoinInfo *info,JoinArg *joinRel);

int isFull(struct Vector *vector);
int isEmpty(struct Vector *vector);

void printVector(struct Vector *vector);
void printTuple(struct Vector *vector,unsigned pos);
uint64_t checkSum(struct Vector *vector,uint64_t *col,unsigned rowIdPos);

#endif