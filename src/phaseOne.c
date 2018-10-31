#include "phaseOne.h"
#include "bitWiseUtil.h"


uint32_t *selectColumn(uint32_t **array, uint32_t columnNumber)
{
	return *(array + columnNumber);
}



void columnPrint(uint32_t *column, uint32_t columnSize)
{
	int i = 0;
	printf("~~\n");
	for (i = 0; i < columnSize; i++)
		printf("%u\n", column[i]);
	printf("~~\n");
}

void printArray(uint32_t *array, uint32_t size)
{
	int i;
	for (i = 0; i < size; i++)
		printf("%u | ", array[i]);
	printf("\n");
}

struct PlaceHolder* convertToStructs(uint32_t *column, uint32_t columnSize)
{
	int i;
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

struct tuple* findSizeOfHistogram(struct PlaceHolder* data, uint32_t columnSize, uint32_t *histSize)
{
	int i;
	int j;
	uint32_t *countArray;
	uint32_t histogramSize = 0;
	countArray = malloc(rangeOfValues * sizeof(uint32_t));
	
	/* Making sure that we have 0 in every element */
	/* We are basicly doing a counting sort*/
	for (i = 0; i < rangeOfValues; ++i) 
		countArray[i] = 0;

	// printArray(countArray, rangeOfValues);
	
	for (i = 0; i < columnSize; ++i) 
		countArray[data[i].hashValue] += 1;

	printArray(countArray, rangeOfValues);

	for (i = 0; i < rangeOfValues; i++)
	{
		if (countArray[i] != 0)
			histogramSize++;
	}

	printf("histogramSize is %u\n", histogramSize);

	struct tuple* histogram = malloc(histogramSize * sizeof(struct tuple));

	i = 0;
	j = 0;
	while(i < rangeOfValues)
	{
		if (countArray[i] > 0)
		{
			/* This is the hashValue */
			histogram[j].key = i;
			/* This is it's occurence */
			histogram[j].payLoad = countArray[i];
			/* Move to the next tuple*/
			j++;
		}
		i++;
	}

	for (i = 0; i < histogramSize; i++)
		printTuple(&(histogram[i]));

	*histSize = histogramSize;
	return histogram;
}


struct tuple* createPsum(struct tuple* hist, uint32_t histSize)
{
	int i;
	uint32_t sum = 0;
	struct tuple* pSum = malloc(histSize * sizeof(struct tuple));	
	for (i = 0; i < histSize; i++)
	{
		printf("sum %u\n", sum );
		/* Get the hashValue */
		pSum[i].key = hist[i].key;
		/* Add the sum */
		pSum[i].payLoad = sum;
		sum += hist[i].payLoad;
	}

	printArrayOfTuples(hist, histSize);	

	printf("&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&\n");

	printArrayOfTuples(pSum, histSize);

	return pSum;
}

struct PlaceHolder* createSecondR(
	struct PlaceHolder* originalR, uint32_t columnSize, struct tuple* pSum, uint32_t pSumSize)
{
	int i;
	/* j indexes pSum*/
	int j = 0;
	struct PlaceHolder *arr = malloc(columnSize * sizeof(struct PlaceHolder));

	for (i = 0; i < columnSize; i++)
	{
		// if (pSum[j] == 0)
		// arr[i].value = column[i];
		// arr[i].hashValue = firstHash(column[i], significantsForHash);
		// arr[i].rowId = i;
		// // printf("%u\n", column[i]);
		
		////////////////////////////////////////////////////////////////////////////////////////////////
		// Pernoume to hashValue tou Placeholder
		uint32_t h = originalR[i].hashValue;
	
		// Pame sto pSum kai vriskoume pou ksekinane ta PlaceHolders ston RTonos
		// me auto to hashValue 
		uint32_t offset = pSum[h];

		// Copy ta pedia apo to ena PlaceHolder [originalR] sto allo [RTonos]
		arr[offset].value 		= originalR[i].value;
		arr[offset].hashValue 	= h;
		arr[offset].rowId 		= originalR[i].rowId;

		// Ta PlaceHolders me auto to hashValue tha kseksinoun pleon, 
		// apo thn epomeni thesi

		// Otan erthei kapoia stigmi se epomeno iteration kapoio PlaceHolder me auto to hashValue
		// that mpei apo katw apo auto pou molis topothetisame.
		pSum[h]++;

		// SHMEIWSH: //
		// Me auton ton tropo "xanoume" ta dedomena tou pSum 
		// Afou ta kanoume increment sthn teleutaia entolh.
		// Dhladh an meta sthn askisi ton xreiazomaste sthn arxikh tou
		// morfi tha prepei na exoume ena copy.
		////////////////////////////////////////////////////////////////////////////////////////////////
	}
}
