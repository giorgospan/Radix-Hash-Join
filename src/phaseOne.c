#include "phaseOne.h"
#include "bitWiseUtil.h"


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

	printArrayOfStructs(arr, columnSize);	

	return arr; 
}

uint32_t* createHistogram(struct PlaceHolder* data, uint32_t columnSize)
{
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	uint32_t i;
	uint32_t *countArray  = malloc(rangeOfValues * sizeof(uint32_t));

	// printArrayOfStructs(data, columnSize);
	
	/* Making sure that we have 0 in every element */
	/* We are basicly doing a counting sort*/

	// printArray(countArray, rangeOfValues);
	
	for (i = 0; i < rangeOfValues; ++i) 
		countArray[i] = 0;

	// printArray(countArray, rangeOfValues);
	
	for (i = 0; i < columnSize; ++i)
		countArray[data[i].hashValue] += 1;

	printArray(countArray, rangeOfValues);

	return countArray;
}


uint32_t** createPsum(uint32_t* hist)
{
	uint32_t i;
	uint32_t sum = 0;
	uint32_t **pSum = malloc(rangeOfValues * sizeof(uint32_t*));

	/* NULL MEANS THE HASH VALUE IS NOT IN R*/
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

	printPsum(pSum);
	return pSum;
}

void printPsum(uint32_t **pSum)
{
	printf("###############################################################################\n");
	int i = 0;
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
	int i = 0;
	for (i = 0; i < rangeOfValues; i++)
	{
		if (pSum[i] != NULL)
			free(pSum[i]);
	}
	free(pSum);
}

struct PlaceHolder* createSecondR(struct PlaceHolder* originalR, uint32_t columnSize, uint32_t** pSum)
{
	uint32_t i;
	struct PlaceHolder *arr = malloc(columnSize * sizeof(struct PlaceHolder));

	for (i = 0; i < columnSize; i++)
	{	
		/* Get the hashValue from R */
		uint32_t h = originalR[i].hashValue;
	
		/* Go to pSum and find where we need to place it in secondR */
		uint32_t offset;
		if (pSum[h] != NULL)
		{
			offset = *(pSum[h]);
			/* Increment the value to know where to put he next element with the same hashValue*/
			/* With this we lose the original representation of pSum*/
			/* If we want to have access we must create a acopy before entering this for loop*/
			(*pSum[h])++;
		}
		else
		{
			/* This basicly never happens because we will never access an element of pSum
			that has it's value NULL, because if it is NULL there is no such hashValue in the originalR */
			printf("Undefined State\n");
			exit(0);
		}

		/* Just copy the fields of the originalR to secondR */
		arr[offset].value 		= originalR[i].value;
		arr[offset].hashValue 	= h;
		arr[offset].rowId 		= originalR[i].rowId;
	}
	/* Now we basicly have created a new array secondR which is the original sorted by it's hashValue */
	printPsum(pSum);
	printArrayOfStructs(arr, columnSize);
	return arr;
}