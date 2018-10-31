#include "usefulHeaders.h"
#include "prepareInput.h"

void inputReader()
{
	FILE *fp = NULL;
	int i;
	int j;
	uint32_t numOfTuples;
	uint32_t numOfColumns;

	fp = fopen("input.bin", "rb");

	fread(&numOfTuples, sizeof(uint32_t), 1, fp);
	printf("Num of Tuples %u\n", numOfTuples);
	fread(&numOfColumns, sizeof(uint32_t), 1, fp);
	printf("Num of Columns %u\n", numOfColumns);

	for (i = 0; i < numOfTuples; ++i)
	{
		for (j = 0; j < numOfColumns; ++j)
		{
			uint32_t r;
			fread(&r, sizeof(uint32_t), 1, fp);
			// printf("Tuple %u Column %u Value %u\n", i, j, r);
			printf("T%d C%d %d\n", i, j, r);
		}
	}

	/* Optional Done String */
	// uint32_t s[410];
	// fread(s, sizeof(s), 1, fp);
	// printf("%s\n", s);

	fclose(fp);
}

void inputCreator()
{
	FILE *fp = NULL;
	int i;
	int j;

	// int32_t numOfTuples = rand() % 10 + 20;
	// int32_t numOfColumns = rand() % 10 + 5;
	uint32_t numOfTuples = 10;
	uint32_t numOfColumns = 3;

	fp = fopen("input.bin", "wb");
	fwrite(&numOfTuples, sizeof(uint32_t), 1, fp);
	fwrite(&numOfColumns, sizeof(uint32_t), 1, fp);

	for (i = 0; i < numOfTuples; ++i)
	{
		for (j = 0; j < numOfColumns; ++j)
		{
			uint32_t r = rand()  % 10 + 30;
			fwrite(&r, sizeof(uint32_t), 1, fp);
		}
	}

	/* Optional Done String */
	// uint32_t s[] = "Done";
	// fwrite(s, sizeof(s), 1, fp);
	
	fclose(fp);
}

void createAndRead()
{
	inputCreator();
	inputReader();
}


void preetyArrayPrint(uint32_t ** arr, uint32_t numOfRows, uint32_t numOfColumns)
{
	int i;
	int j;

	for (i = 0; i < numOfRows + 15; ++i) printf("=");
	printf("\n");

	for (i = 0; i < numOfRows; ++i)
	{
		printf("| ");
		for (j = 0; j < numOfColumns; ++j)
			printf("%u ", arr[i][j]);
		printf("|\n");
	}

	for (i = 0; i < numOfRows + 15; ++i) printf("=");
	printf("\n");
}

uint32_t ** allocateArray(uint32_t numOfRows, uint32_t numOfColumns)
{
	int i;
	int j;
	uint32_t **arr = malloc(numOfRows * sizeof(uint32_t*));
	for (i = 0; i < numOfRows; ++i)
		arr[i] = malloc(numOfColumns * sizeof(uint32_t));

	// preetyArrayPrint(arr, numOfRows, numOfColumns);
	return arr;
}

void deAllocateArray(uint32_t **array, uint32_t numOfRows)
{
	int i = 0;
	for (i = 0; i < numOfRows; i++)
		free(array[i]);
	free(array);
}

uint32_t** createArrayAndInit(uint32_t *rowSize, uint32_t *colSize)
{
	FILE *fp = NULL;
	int i;
	int j;
	uint32_t numOfTuples;
	uint32_t numOfColumns;
	uint32_t **array = NULL;

	/* Open the file and get the num of Columns */

	fp = fopen("input.bin", "rb");

	fread(&numOfTuples, sizeof(uint32_t), 1, fp);
	printf("Num of Tuples %u\n", numOfTuples);
	fread(&numOfColumns, sizeof(uint32_t), 1, fp);
	printf("Num of Columns %u\n", numOfColumns);

	/* First we allocate the array */
	array = allocateArray(numOfColumns, numOfTuples);

	/* Then we fill it */
	for (i = 0; i < numOfTuples; ++i)
	{
		for (j = 0; j < numOfColumns; ++j)
		{
			uint32_t r;
			fread(&r, sizeof(uint32_t), 1, fp);
			/* array[j][i] in order to have all the elemets of a column in the row
				it makes sense in the way we allocated the array */	
			array[j][i] = r;
		}
	}

	/* The index trick to have it in a pseudo column major index*/
	*rowSize = numOfColumns;
	*colSize = numOfTuples;
	preetyArrayPrint(array, numOfColumns, numOfTuples);
	
	fclose(fp);
	return array;
}