#include "arrayUtil.h"



void prettyArrayPrint(uint64_t **arr, uint64_t numOfRows, uint64_t numOfColumns)
{

	/* Unfortunatelly, it doesn't print array in a pretty way 
		if the numbers are big [i.e:many digits long]	*/
	uint64_t i;
	uint64_t j;

	printf("This is how we store the relation: \n");
	printf(" ");
	for (i = 0; i < 3*numOfColumns; ++i) printf("=");
	printf("\n");

	for (i = 0; i < numOfRows; ++i)
	{
		printf("| ");
		for (j = 0; j < numOfColumns; ++j)
			printf("%20llu ", arr[i][j]);
		printf("|\n");
	}

	printf(" ");
	for (i = 0; i < 3*numOfColumns; ++i) printf("=");
	printf("\n");
}

uint64_t** allocateArray(uint64_t numOfRows, uint64_t numOfColumns)
{
	uint64_t i;
	uint64_t j;
	uint64_t **arr = malloc(numOfRows * sizeof(uint64_t*));
	for (i = 0; i < numOfRows; ++i)
		arr[i] = malloc(numOfColumns * sizeof(uint64_t));

	// prettyArrayPrint(arr, numOfRows, numOfColumns);
	return arr;
}

void deAllocateArray(uint64_t **array, uint64_t numOfRows)
{
	uint64_t i = 0;
	for (i = 0; i < numOfRows; i++)
		free(array[i]);
	free(array);
}

void deAllocateArrayChar(char **array, uint64_t size)
{
	uint64_t i = 0;
	for (i = 0; i < size; i++)
		free(array[i]);
	free(array);
}

