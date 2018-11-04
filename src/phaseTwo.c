#include "usefulHeaders.h"
#include "structDefinitions.h"
#include "phaseTwo.h"
#include "phaseOne.h" /* For accessing rangeOfValues variable */

#include <unistd.h> /* For debugging */

/* It'll change if we change secondHash(..) */
uint32_t rangeOfHash2=1024;

void initializeIndexArray(struct relation* R)
{
	uint32_t i,j;
	uint32_t bucketSize;

	/* Firstly, we need to allocate space for indexArray */	
	/* Remember: One struct index per bucket */
	R->indexArray = malloc(rangeOfValues*sizeof(struct index*));

	// For every bucket
	for(i=0;i<rangeOfValues;i++)
	{
		// If bucket is empty, its index will be NULL
		if(R->histoGram[i] == 0)
			R->indexArray[i] = NULL;

		else
		{
			// Fetch bucket's size from histoGram array
			// Remember: histoGram is array with ints
			bucketSize = R->histoGram[i];

			/* Allocate space for bucket's index */
			R->indexArray[i] = malloc(sizeof(struct index));

			// Allocate space for its index's fields
			R->indexArray[i]->chainArray = malloc(bucketSize*sizeof(uint32_t));
			R->indexArray[i]->bucketArray = malloc(rangeOfHash2*sizeof(uint32_t));

			/* Initialize chainArray and bucketArray with 0's */
			for(j=0;j<bucketSize;j++)
				R->indexArray[i]->chainArray[j] = 0;

			for(j=0;j<rangeOfHash2;j++)
				R->indexArray[i]->bucketArray[j] = 0;
		}
	}
}

void buildIndexPerBucket(struct relation* R)
{
	uint32_t i;
	uint32_t bucketSize;
	uint32_t hash;
	int32_t j;
	int32_t k;
	int32_t c;
	int32_t start;

	printf("\nHistogram of small relation:\n");
	printArray(R->histoGram, rangeOfValues);

	printf("\n~~~~~~~~~SMALL~~~~~~~~~\n");
	printf("Number of rows:%d\n",R->rows);
	printf("Number of cols:%d\n",R->cols);

	// For every bucket
	for(i=0;i<rangeOfValues;i++)
	{
		/* If bucket is not empty[i.e: has an index] */
		if(R->indexArray[i] != NULL)
		{

			// Fetch bucket's starting point from pSum array
			// Remember: pSum is array with pointers to int
			start = *(R->pSum[i]);

			// Fetch bucket's size from histoGram array
			// Remember: histoGram is array with ints
			bucketSize = R->histoGram[i];

			// printf("[%d]bucketSize:%d\n",i,bucketSize);

			/* Scan from the bottom of the bucket to the top */
			for(j=start+bucketSize-1;j>=start;j--)
			{
				hash = secondHash(R->final[j].value);
				// printf("\nsecondHash(%d): %d\n",R->final[j].value,hash);

				if(R->indexArray[i]->bucketArray[hash] == 0)
				{
					// printf("Found empty spot in bucketArray\n");
					R->indexArray[i]->bucketArray[hash] = (j-start)+1;
					// printf("bucketArray[%d]: %d\n",hash,  R->indexArray[i]->bucketArray[hash] );
				}
				else
				{
					/* Find the first zero in chainArray 
						by following the chain and
						save "j" in that place */
					k = R->indexArray[i]->bucketArray[hash]-1;
					// printf("Moving to chainArray[%d](now is equal to %d)\n",k,R->indexArray[i]->chainArray[k]);
					while(1)
					{	
						// We've found an empty spot in chainArray 
						if(R->indexArray[i]->chainArray[k] == 0)
						{	
							R->indexArray[i]->chainArray[k] = (j-start) + 1;
							// printf("Found empty spot on chainArray[%d]\n",k);
							break;
						}
						/* Step further on the chain */
						else
						{
							k = R->indexArray[i]->chainArray[k] - 1;
							// printf("Moving to chainArray[%d](now is equal to %d)\n",k,R->indexArray[i]->chainArray[k]);
							// sleep(1);
						}
					}
				}
			}	
		}
		// else
		// 	printf("Empty bucket\n");
		// printf("==============================================\n");
	}
}


uint32_t secondHash(uint32_t num)
{
	// return num % 8;
	return num & ((1<<10)-1); 
	// return rand() %8;
}

void deleteIndexArray(struct index** indArray)
{
	uint32_t i;

	/* Make sure relation has an indexArray*/
	if(indArray==NULL)
		return;

	/* For every bucket of the relation */
	for(i=0;i<rangeOfValues;i++)
	{
		/* If this bucket has an index */  
		if(indArray[i] != NULL)
		{
			/* Free index fields */
			free(indArray[i]->chainArray);
			free(indArray[i]->bucketArray);

			/* Free index struct itself */
			free(indArray[i]);
		}
	}

	/* Free indexArray of the relation*/
	free(indArray);
}

//////// Debugging purposes////////////////////////
// void foo(struct index** indArray)
// {
// 	uint32_t i;

// 	for(i=0;i<rangeOfValues;i++)
// 	{
// 		printf("indArray[%d]:%p\n",i,indArray[i]);
// 	}
// }
/////////////////////////////////////////////////
