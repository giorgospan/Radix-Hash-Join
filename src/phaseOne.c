#include "phaseOne.h"
#include "bitWiseUtil.h"

const int significantsForHash = 5;
const int rangeOfValues = 32;


uint32_t *selectColumn(uint32_t **array, uint32_t columnNumber)
{
	return *(array + columnNumber);
}



void columnPrint(uint32_t *column, uint32_t columnSize)
{
	uint32_t i = 0;
	printf("~~\n");
	for (i = 0; i < columnSize; i++)
		printf("%u\n", column[i]);
	printf("~~\n");
}

void printArray(uint32_t *array, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; i++)
		printf("%u | ", array[i]);
	printf("\n");
}

struct PlaceHolder* convertToStructs(uint32_t *column, uint32_t columnSize)
{
	uint32_t i;
	struct PlaceHolder *arr = malloc(columnSize * sizeof(struct PlaceHolder));
	for (i = 0; i < columnSize; i++)
	{
		arr[i].value = column[i];
		arr[i].hashValue = firstHash(column[i], significantsForHash);
		arr[i].rowId = i;
		// printf("%u\n", column[i]);
	}

	// printArrayOfStructs(arr, columnSize);	
	return arr; 
}

uint32_t* createHistogram(struct PlaceHolder* data, uint32_t columnSize)
{
	// printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	uint32_t i;
	uint32_t *countArray  = malloc(rangeOfValues * sizeof(uint32_t));

	// printArrayOfStructs(data, columnSize);
	
	/* Making sure that we have 0 in every element */
	/* We are basically doing a counting sort*/

	for (i = 0; i < rangeOfValues; ++i) 
		countArray[i] = 0;

	for (i = 0; i < columnSize; ++i)
		countArray[data[i].hashValue] += 1;

	printf("\nHistogram:\n");
	printArray(countArray, rangeOfValues);

	return countArray;
}


uint32_t** createPsum(uint32_t* hist)
{
	uint32_t i;
	uint32_t sum = 0;
	uint32_t **pSum = malloc(rangeOfValues * sizeof(uint32_t*));

	/* NULL MEANS THE HASH VALUE IS NOT IN ARRAY*/
	for (i = 0; i < rangeOfValues; i++)
		pSum[i] = NULL;
	
	for (i = 0; i < rangeOfValues; i++)
	{
		if (hist[i] != 0)
		{
			pSum[i] = malloc(sizeof(uint32_t));
			*(pSum[i]) = sum;
			sum += hist[i];
		}
	}

	// printf("\nPrefixSum:\n");
	// printPsum(pSum);
	return pSum;
}

void printPsum(uint32_t **pSum)
{
	// printf("###############################################################################\n");
	uint32_t i = 0;
	for (i = 0; i < rangeOfValues; i++)
	{
		if (pSum[i] == NULL)
			printf("index %u NULL\n", i);
		else
			printf("index %u %u\n", i, *(pSum[i]));
	}
}

void deletepSum(uint32_t **pSum)
{
	uint32_t i = 0;
	for (i = 0; i < rangeOfValues; i++)
	{
		if (pSum[i] != NULL)
			free(pSum[i]);
	}
	free(pSum);
}

struct PlaceHolder* sortArray(struct PlaceHolder* original, uint32_t columnSize, uint32_t** pSum)
{
	uint32_t i;
	struct PlaceHolder *arr = malloc(columnSize * sizeof(struct PlaceHolder));

	/* Creating a copy of pSum, because we're gonna need it later in phaseTwo */
	uint32_t **pSumCopy = malloc(rangeOfValues * sizeof(uint32_t*));
	for (i = 0; i < rangeOfValues; i++)
	{
		pSumCopy[i] = NULL;
		if (pSum[i] != NULL)
		{
			pSumCopy[i] = malloc(sizeof(uint32_t));
			*pSumCopy[i] = *pSum[i];
		}
	}

	for (i = 0; i < columnSize; i++)
	{	
		/* Get the hashValue from original array */
		uint32_t h = original[i].hashValue;
	
		/* Go to pSumCopy and find where we need to place it in second(i.e: final) array */
		uint32_t offset;
		if (pSumCopy[h] != NULL)
		{
			offset = *(pSumCopy[h]);
			/* Increment the value to know where to put he next element with the same hashValue*/
			/* With this we lose the original representation of pSum*/
			/* If we want to have access we must create a acopy before entering this for loop*/
			(*pSumCopy[h])++;
		}
		else
		{
			/* This basically never happens because we will never access an element of pSumCopy
			that has its value NULL, because if it is NULL there is no such hashValue in the original array */
			printf("Undefined State\n");
			exit(1);
		}

		/* Just copy the fields of the original array to the second one */
		arr[offset].value 		= original[i].value;
		arr[offset].hashValue 	= h;
		arr[offset].rowId 		= original[i].rowId;
	}
	/* Delete copy */
	deletepSum(pSumCopy);

	/* Now we basically have created a new array,second array, which is the original sorted by its hashValue */
	/* We return it */
	return arr;
}