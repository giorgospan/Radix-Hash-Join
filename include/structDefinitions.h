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

	// + some stuff for phaseTwo [will be added in a later commit]
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

/* Just free the memory
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