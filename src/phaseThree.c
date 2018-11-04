#include "usefulHeaders.h"
#include "structDefinitions.h"
#include "bitWiseUtil.h"
#include "phaseOne.h"
#include "phaseTwo.h"
#include "phaseThree.h"
#include "list.h"


void findResult(struct relation* big,struct relation* small,struct List *list)
{

	uint32_t i;
	uint32_t searchBucket;
	uint32_t searchValue;
	uint32_t hash;
	uint32_t start;
	int32_t k;
	struct index *searchIndex;

	/* For every tuple(i.e:record) in the big relation */
	for(i=0;i<big->cols;i++)
	{
		/* The value we are gonna search for */
		searchValue = big->final[i].value;

		/* Find out in which bucket of the small relation
			we should search */
		searchBucket = firstHash(searchValue,significantsForHash);

		/* Bucket is empty, there is nothing to search here */
		if(small->pSum[searchBucket] == NULL)
			continue;

		/* Fetch starting point of the bucket */
		start = *(small->pSum[searchBucket]);

		/* Fetch the index of this bucket */
		searchIndex = small->indexArray[searchBucket];

		/* Find out where to look for in the bucketArray of the index */
		hash = secondHash(searchValue);

		/* Bucket is not empty, but there is no value equal to the searchValue */
		if(searchIndex->bucketArray[hash]==0)
			continue;

		/* Warning: In bucketArray and chainArray we've stored the rowIds relevant to the bucket [i.e: 0 ~> bucketSize-1] */

		k = searchIndex->bucketArray[hash] - 1;
		checkEqual(small,big->final[i].rowId,start,searchValue,k,list);

		while(1)
		{	
			// We've reached the end of the chain
			if(searchIndex->chainArray[k] == 0)
				break;

			/* Step further on the chain */
			else
			{
				k = searchIndex->chainArray[k] - 1;
				checkEqual(small,big->final[i].rowId,start,searchValue,k,list);
			}
		}

	}

}

void checkEqual(struct relation* small,uint32_t i,uint32_t start,uint32_t searchValue,uint32_t pseudoRow,struct List* list)
{
	uint32_t actualRow;
	struct resultTuple tuple;

	/* We calculate the rowId relevant to the final array [i.e: 0 ~> ncols] */
	actualRow = start + pseudoRow;

	if(small->final[actualRow].value == searchValue)
	{
		tuple.rowId1 = i;
		tuple.rowId2 = small->final[actualRow].rowId;
		ListInsert(list,&tuple);
	}
}



