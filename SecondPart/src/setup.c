#include "setup.h"
#include "arrayUtil.h"
#include "structDefinitions.h"

void setup()
{
	uint64_t numOfRelations;
	struct placeHolder *interfaceArray = NULL;
	char **relationNames = readRelationFileNames(&numOfRelations);
	uint64_t*** huge = mapRelationsToMemory(relationNames, numOfRelations, &interfaceArray);

	/* Deallocates*/
	deAllocateArrayChar(relationNames, numOfRelations);
	freeRelationArray(interfaceArray, numOfRelations);
	printf("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
	printf("interfaceArray %p\n", interfaceArray);
	// printPlaceholder(&interfaceArray[0]);
	// free(interfaceArray);
	int res = munmap(huge, numOfRelations);
	printf("res %d\n", res);
}


char** readRelationFileNames(uint64_t *numOfRelations)
{
	char buffer[20];
	FILE *fp = NULL;
	int i;
	char c;
	*numOfRelations = 0;

	// Counting the lines of the file
	if ((fp = fopen("workloads/small/small.init", "r")) == NULL)
	{
		perror("fopen failed[readRelationFileNames]");
		exit(EXIT_FAILURE);
	}

	while( (c = getc(fp)) != EOF )
		if (c == '\n')
			(*numOfRelations)++;
	printf("Counted %llu lines\n", *numOfRelations);

	fclose(fp);

	/* Reopen to get the names of the relations */
	char **arr = NULL;
	arr = malloc(sizeof(char*) * (*numOfRelations));
	if ((fp = fopen("workloads/small/small.init", "r")) == NULL)
	{
		perror("fopen failed[readRelationFileNames]");
		exit(EXIT_FAILURE);
	}

	i = 0;
	while (fgets(buffer, sizeof(buffer), fp) != NULL)
	{
		/* Remove '\n from string */
		buffer[strcspn(buffer, "\n")] = 0;
		// printf("buffer : %s\n", buffer);
		// printf("strlen of buffer %d\n", strlen(buffer));
		arr[i] = malloc(sizeof(char) * (strlen(buffer) + 1));
		strcpy(arr[i], buffer);	
		// printf("HERE %s\n", arr[i]);
		i++;
	}

	fclose(fp);
	return arr;
}

uint64_t*** mapRelationsToMemory(char **relationNames, uint64_t numOfRelations, struct placeHolder **interfaceArray)
{
	FILE *fp = NULL;
	const char *path = "workloads/small";
	char actualPath[30];
	int i;
	int j;

	caddr_t memory =  mmap(NULL, numOfRelations * sizeof(uint64_t**), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
	if (memory == MAP_FAILED)
	{
		perror("mmap FAILED[mapRelationsToMemory]");
		exit(EXIT_FAILURE);
	}

	printf("%p\n", memory);
	uint64_t ***hugeArray = (uint64_t***) memory;

	/* JUST A DEMO TO SEE WE KNOW WHAT WE ARE DEALING WITH */
	// hugeArray = malloc(sizeof(uint64_t**) * numOfRelations);
	// hugeArray[0] = malloc(sizeof(uint64_t*) * 5); 
	// hugeArray[0][0] = malloc(sizeof(uint64_t)); 
	// printf("%p\n", hugeArray);
	// hugeArray[0][0][0] = 139;
	// printf("%llu\n", hugeArray[0][0][0]);


	/* Create the array with the extra information */
	struct placeHolder *intermediate = initRelationArray(numOfRelations);
	printf("intermediate %p\n", intermediate);
	for (i = 0; i < numOfRelations; i++)
	{
		/* Create the path to read the relation */
		strcpy(actualPath, path);
		strcat(actualPath, "/");
		strcat(actualPath, relationNames[i]);
		// printf("%s\n", actualPath);

		if ((fp = fopen(actualPath, "rb")) == NULL)
		{
			perror("fopen failed[mapRelationsToMemory]");
			exit(EXIT_FAILURE);
		}
		// interfaceArray[i] = malloc(sizeof(struct placeHolder));
		hugeArray[i] = createArrayAndInit(&(intermediate[i].numOfRows), &(intermediate[i].numOfColumns), fp);
		// printf("problem %llu\n",  intermediate[i].numOfRows);
		// printf("problem %llu\n",  intermediate[i].numOfColumns);
		intermediate[i].arrayPointer = malloc(sizeof(uint64_t*) * intermediate[i].numOfRows);
		// printPlaceholder(&(intermediate[i]));
		for (j = 0; j < intermediate[i].numOfRows; j++)
		{
			*((intermediate[i].arrayPointer) + j) = hugeArray[i][j];
		}
		// printPlaceholder(&(intermediate[i]));
		fclose(fp);
	}

	printf("intermediate %p\n", intermediate);
	for (i = 0; i < numOfRelations; i++)
		printPlaceholder(&(intermediate[i]));

	printPlaceholder(intermediate);
	*interfaceArray = intermediate;
	return hugeArray;
}


uint64_t** createArrayAndInit(uint64_t *rowSize, uint64_t *colSize, FILE *fp)
{
	uint64_t i;
	uint64_t j;
	uint64_t numOfTuples;
	uint64_t numOfColumns;
	uint64_t **array = NULL;

	fread(&numOfTuples, sizeof(uint64_t), 1, fp);
	printf("Num of Tuples %llu\n", numOfTuples);
	fread(&numOfColumns, sizeof(uint64_t), 1, fp);
	printf("Num of Columns %llu\num", numOfColumns);

	/* First we allocate the array */
	array = allocateArray(numOfColumns, numOfTuples);

	/* Then we fill it */
	for (i = 0; i < numOfTuples; ++i)
	{
		for (j = 0; j < numOfColumns; ++j)
		{
			uint64_t r;
			fread(&r, sizeof(uint64_t), 1, fp);
			/* array[j][i] in order to have all the elemets of a column in the row
				it makes sense in the way we allocated the array */	
			array[j][i] = r;
		}
	}

	/* The index trick to have it in a pseudo column major index*/
	*rowSize = numOfColumns;
	*colSize = numOfTuples;
	// prettyArrayPrint(array, numOfColumns, numOfTuples);
	
	/* WEIRD FOR SOME REASON IT PRINTS um3 instead of 3*/
	// printf("%llu \n", *rowSize);
	// printf("colSize %llu \n", *colSize);
	return array;
}