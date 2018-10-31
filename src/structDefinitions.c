#include "structDefinitions.h"

void deAllocateStructs(struct PlaceHolder* structToDelete)
{
	free(structToDelete);
}


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
	int i;
	for (i = 0; i < size; i++)
		printTuple(&(arrayToPrint[i]));
}

void printArrayOfStructs(struct PlaceHolder* arrayToPrint, uint32_t size)
{
	int i;
	for (i = 0; i < size; i++)
		printStruct(&(arrayToPrint[i]));
}