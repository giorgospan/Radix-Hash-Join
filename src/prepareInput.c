#include "usefulHeaders.h"
#include "prepareInput.h"

void inputReader()
{
	FILE *fp = NULL;
	uint32_t i;
	uint32_t j;
	uint32_t numOfTuples;
	uint32_t numOfColumns;

	if((fp = fopen("input.bin", "rb")) == NULL)
	{
		perror("fopen failed[inputReader]");
		exit(1);	
	}

	fread(&numOfTuples, sizeof(uint32_t), 1, fp);
	// printf("Num of Tuples %u\n", numOfTuples);
	fread(&numOfColumns, sizeof(uint32_t), 1, fp);
	// printf("Num of Columns %u\n", numOfColumns);

	for (i = 0; i < numOfTuples; ++i)
	{
		for (j = 0; j < numOfColumns; ++j)
		{
			uint32_t r;
			fread(&r, sizeof(uint32_t), 1, fp);
			// printf("Tuple %u Column %u Value %u\n", i, j, r);
			printf("T%d C%d %d | ", i, j, r);
		}
		printf("\n");
	}
	printf("\n");

	/* Optional Done String */
	// uint32_t s[410];
	// fread(s, sizeof(s), 1, fp);
	// printf("%s\n", s);

	fclose(fp);
}

void inputCreator()
{
	FILE *fp = NULL;
	uint32_t i;
	uint32_t j;

	// uint32_t numOfTuples = rand()  % 9000001 + 1000000;	// Range: 1.000.000 ~> 10.000.000
	uint32_t numOfTuples = 500000;
	uint32_t numOfColumns = rand() % 4 + 2; // Range: 2 ~> 5

	if((fp = fopen("input.bin", "wb")) == NULL)
	{
		perror("fopen failed[inputCreator]");
		exit(1);	
	}
	fwrite(&numOfTuples, sizeof(uint32_t), 1, fp);
	fwrite(&numOfColumns, sizeof(uint32_t), 1, fp);

	for (i = 0; i < numOfTuples; ++i)
	{
		for (j = 0; j < numOfColumns; ++j)
		{
			// uint32_t r = rand()  % 100;	 // Range: 0 ~> 100
			// uint32_t r = rand()  % 10000; // Range: 0 ~> 10.000
			uint32_t r = rand()  % 100000; // Range: 0 ~> 100.000
			// uint32_t r = rand()  % 10000000; // Range: 0 ~> 10.000.000
			fwrite(&r, sizeof(uint32_t), 1, fp);
		}
	}

	/* Optional "Done" String */
	// uint32_t s[] = "Done";
	// fwrite(s, sizeof(s), 1, fp);
	
	fclose(fp);
}

void createAndRead()
{
	inputCreator();
	// inputReader();
}


void prettyArrayPrint(uint32_t **arr, uint32_t numOfRows, uint32_t numOfColumns)
{

	/* Unfortunatelly, it doesn't print array in a pretty way 
		if the numbers are big [i.e:many digits long]	*/
	uint32_t i;
	uint32_t j;

	printf("This is how we store the relation: \n");
	printf(" ");
	for (i = 0; i < 3*numOfColumns; ++i) printf("=");
	printf("\n");

	for (i = 0; i < numOfRows; ++i)
	{
		printf("| ");
		for (j = 0; j < numOfColumns; ++j)
			printf("%u ", arr[i][j]);
		printf("|\n");
	}

	printf(" ");
	for (i = 0; i < 3*numOfColumns; ++i) printf("=");
	printf("\n");
}

uint32_t** allocateArray(uint32_t numOfRows, uint32_t numOfColumns)
{
	uint32_t i;
	uint32_t j;
	uint32_t **arr = malloc(numOfRows * sizeof(uint32_t*));
	for (i = 0; i < numOfRows; ++i)
		arr[i] = malloc(numOfColumns * sizeof(uint32_t));

	// prettyArrayPrint(arr, numOfRows, numOfColumns);
	return arr;
}

void deAllocateArray(uint32_t **array, uint32_t numOfRows)
{
	uint32_t i = 0;
	for (i = 0; i < numOfRows; i++)
		free(array[i]);
	free(array);
}

uint32_t** createArrayAndInit(uint32_t *rowSize, uint32_t *colSize)
{
	FILE *fp = NULL;
	uint32_t i;
	uint32_t j;
	uint32_t numOfTuples;
	uint32_t numOfColumns;
	uint32_t **array = NULL;

	/* Open the file and get the num of Columns */
	if((fp = fopen("input.bin", "rb")) == NULL)
	{
		perror("fopen failed[createArrayAndInit]");
		exit(1);	
	}

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
	// prettyArrayPrint(array, numOfColumns, numOfTuples);
	
	fclose(fp);
	return array;
}