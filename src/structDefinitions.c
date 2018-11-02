#include "structDefinitions.h"
#include "prepareInput.h"
#include "phaseOne.h"

void initializeRelation(struct relation** R)
{
	// Allocate space for struct relation
	*R = malloc(sizeof(struct relation));

	// Create input file [and print it]
	createAndRead();

	// Create relation array [and print it]
	(*R)->inputRelation = createArrayAndInit(&(*R)->rows,&(*R)->cols);
	// prettyArrayPrint((*R)->inputRelation, (*R)->rows, (*R)->cols);

	// Pick which column we want to join
	// This is just the number of the column
	(*R)->joinCol = 0;

	// Set the rest of the fields equal to NULL [optional]
	(*R)->demi = NULL;
	(*R)->histoGram = NULL;
	(*R)->pSum = NULL;
	(*R)->final = NULL;
}

void deleteRelation(struct relation* R)
{
	free(R->demi);
	free(R->histoGram);
	deletepSum(R->pSum);
	free(R->final);
	deAllocateArray(R->inputRelation, R->rows);
	free(R);
}

////////////////////////////////////////////////////////////
//void deAllocateStructs(struct PlaceHolder* structToDelete)
//{															
//	free(structToDelete);									
//}															
////////////////////////////////////////////////////////////

void printStruct(struct PlaceHolder* structToPrint)
{
	printf(":::::::::::::::::::::::::\n");
	printf("Struct\n");
	printf("rowId %u\n", structToPrint->rowId);
	printf("value %u\n", structToPrint->value);
	printf("hashValue %u\n", structToPrint->hashValue);
}

void printTuple(struct tuple* structToPrint)
{
	printf(":::::::::::::::::::::::::\n");
	printf("Tuple\n");
	printf("key %u\n", structToPrint->key);
	printf("payload %u\n", structToPrint->payLoad);
}

void printArrayOfTuples(struct tuple* arrayToPrint, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; i++)
		printTuple(&(arrayToPrint[i]));
}

void printArrayOfStructs(struct PlaceHolder* arrayToPrint, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; i++)
		printStruct(&(arrayToPrint[i]));
}