#include "structDefinitions.h"
#include "usefulHeaders.h"
#include "arrayUtil.h"


struct placeHolder* initRelationArray(uint64_t numOfRelations)
{
	struct placeHolder *arr = NULL;
	if ( (arr = malloc(numOfRelations * sizeof(struct placeHolder))) == NULL)
	{
		perror("Error when allocating the array that points to relations in memory");
		exit(1);
	}
	else
		return arr;
}


void freeRelationArray(struct placeHolder *array, uint64_t numOfRelations)
{
	int i = 0;
	for (i = 0; i < numOfRelations; i++)
		deAllocateArray(array[i].arrayPointer, array[i].numOfRows);
	free(array);
}


void printPlaceholder(struct placeHolder* st)
{
	int i;
	printf("----------------------------------------------------\n");
	printf("st->numOfRows %llu\n", st->numOfRows);
	printf("st->numOfColumns %llu\n", st->numOfColumns);
	printf("&(st->numOfRows %p\n", &(st->numOfRows));
	printf("&(st->numOfColumns %p\n", &(st->numOfColumns));
	for (i = 0; i < st->numOfRows; i++)
		printf("|  %p   ", st->arrayPointer[i]);
	printf("\n");
}