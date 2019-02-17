#ifndef H_DEFINITIONS
#define H_DEFINITIONS

#include "usefulHeaders.h"

struct PlaceHolder
{
	uint32_t value;
	uint32_t hashValue;
	uint32_t rowId;
};

struct tuple
{
	uint32_t key;
	uint32_t payLoad;
};

/* We could also use the struct tuple from above */
struct resultTuple
{
	/* rowId from R relation */
	uint32_t rowId1;
	/* rowId from S relation */
	uint32_t rowId2;
};


struct relation
{
	uint32_t **inputRelation;
	uint32_t rows;
	uint32_t cols;
	uint32_t joinCol;

	struct PlaceHolder *demi;
	uint32_t *histoGram;
	uint32_t **pSum;
	struct PlaceHolder *final;

	/* One struct index per bucket.
		A relation has many buckets,
		consequently we need an array of indexes */
	struct index **indexArray;
};

struct index
{
	/* These are the two arrays
		used for the indexing of a single bucket */
	uint32_t *chainArray;
	uint32_t *bucketArray;
};


/* Just Print the damn thing
	Pass by refrence for less overhead */
void printStruct(struct PlaceHolder* structToPrint);

/* Just Print the damn tuple
	Pass by refrence for less overhead */
void printTuple(struct tuple* structToPrint);

/* Print the array of tuples*/
void printArrayOfTuples(struct tuple* arrayToPrint, uint32_t size);

/* Print the array of structs*/
void printArrayOfStructs(struct PlaceHolder* arrayToPrint, uint32_t size);

/* Just frees the memory
	OBSELETE */
// void deAllocateStructs(struct PlaceHolder* structToDelete);

/* Create a new struct relation object
	and initialize its fields */
void initializeRelation(struct relation** R);

/* Deallocate the space allocated
	for the fields of the relation
	and of course the relation itself */
void deleteRelation(struct relation* R);

#endif
