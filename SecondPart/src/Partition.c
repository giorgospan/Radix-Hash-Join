#include <stdlib.h>
#include <stdio.h>

#include "Partition.h"
#include "Intermediate.h"
#include "Utils.h"
#include "Vector.h"

RadixHashJoinInfo* partition(JoinArg *joinRel)
{
	RadixHashJoinInfo *info = allocate(sizeof(RadixHashJoinInfo),"partition1");
	info->columnSize		= joinRel->numOfTuples;
	info->tupleSize 		= joinRel->tupleSize;
	info->unsorted          = allocate(sizeof(ColumnInfo),"partition2");
	info->unsorted->values  = allocate(info->columnSize*sizeof(uint64_t),"partition3");
	info->unsorted->rowIds  = allocate(info->columnSize*sizeof(unsigned),"partition4");
	info->indexArray        = NULL;

	// Get values-rowIds from vector
	if(joinRel->isInInter)
	{
		createVector(&info->unsorted->tuples,info->tupleSize);
		scanJoin(info,joinRel);
		// printVector(info->unsorted->tuples);
		// printf("Created join column[unsorted] with %u tuples\n",getVectorTuples(info->unsorted->tuples));
	}

 	// Get values-rowIds from original table
	else
	{
		createVector(&info->unsorted->tuples,1);
		for(unsigned i=0;i<info->columnSize;++i)
		{
			info->unsorted->values[i] = joinRel->col[i];
			insertAtVector(info->unsorted->tuples,&i);
			info->unsorted->rowIds[i] = i;
		}
	}
	// printf("\nUNSORTED COLUMN\n");
	// for(unsigned i=0;i<15;++i)
	// {
	// 	printf("%u | %lu | hash:%lu\n",info->unsorted->rowIds[i],info->unsorted->values[i],HASH_FUN_1(info->unsorted->values[i]));
	// }
	// Create Histogram
	info->hist = allocate(HASH_RANGE_1*sizeof(unsigned),"partition5");
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		info->hist[i] = 0;

	for(unsigned i=0;i<info->columnSize;++i)
		info->hist[HASH_FUN_1(info->unsorted->values[i])] += 1;

	// Create Psum
	unsigned sum = 0;
	info->pSum   = allocate(HASH_RANGE_1*sizeof(unsigned),"partition6");

	for(unsigned i=0;i<HASH_RANGE_1;++i)
		info->pSum[i] = 0;

	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		info->pSum[i] = sum;
		sum += info->hist[i];
	}
	// Sort column [bucket sort]
	sortColumn(info);

	// Destroy unsorted column.
	// From now on, we just need the sorted one.
	destroyColumnInfo(&info->unsorted);

	// Return all the info we produced
	return info;
}

void sortColumn(RadixHashJoinInfo *info)
{
	info->sorted         = allocate(sizeof(ColumnInfo),"sortColumn1");
	info->sorted->values = allocate(info->columnSize*sizeof(uint64_t),"sortColumn2");
	info->sorted->rowIds = allocate(info->columnSize*sizeof(unsigned),"sortColumn3");
	unsigned *pSumCopy   = copyPsum(info->pSum);
	createVectorFixedSize(&info->sorted->tuples,info->tupleSize,info->columnSize);

	for(unsigned i=0;i<info->columnSize;++i)
	{	
		/* Get the hashValue from unsorted column */
		unsigned h = HASH_FUN_1(info->unsorted->values[i]);

		/* Go to pSumCopy and find where we need to place it in sorted column  */
		unsigned offset;
		offset = pSumCopy[h];

		/**
		 * Increment the value to know where to put he next element with the same hashValue
		 * With this we lose the original representation of pSum
		 * If we want to have access we must create a acopy before entering this for loop
		 */
		pSumCopy[h]++;
		info->sorted->values[offset] = info->unsorted->values[i];
		info->sorted->rowIds[offset] = info->unsorted->rowIds[i];
		insertAtPos(info->sorted->tuples,getTuple(info->unsorted->tuples,i),offset);
	}

	// printf("\nSORTED COLUMN\n");
	// for(unsigned i=0;i<15;++i)
	// {
	// 	printf("%u | %lu | hash:%lu\n",info->sorted->rowIds[i],info->sorted->values[i],HASH_FUN_1(info->sorted->values[i]));
	// }
	// printf("Created join column[sorted] with %u tuples\n",getVectorTuples(info->sorted->tuples));
	free(pSumCopy);
}

unsigned *copyPsum(unsigned *original)
{
	unsigned *pSumCopy = allocate(HASH_RANGE_1*sizeof(unsigned),"copyPsum");
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		pSumCopy[i] = original[i];
	return pSumCopy;
}

void destroyRadixHashJoinInfo(RadixHashJoinInfo *info)
{
	destroyColumnInfo(&info->unsorted);
	destroyColumnInfo(&info->sorted);
	free(info->hist);
	free(info->pSum);

	/* For every bucket of the relation */
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		/* If we've created an array of indexes for this relation */
		if(info->indexArray)
			/* If this bucket has an index */
			if(info->indexArray[i] != NULL)
			{
				/* Free index fields */
				free(info->indexArray[i]->chainArray);
				free(info->indexArray[i]->bucketArray);

				/* Free index struct itself */
				free(info->indexArray[i]);
			}
	free(info->indexArray);
	free(info);
}

void destroyColumnInfo(ColumnInfo **c)
{
	if(*c)
	{
		free((*c)->values);
		free((*c)->rowIds);
		destroyVector(&(*c)->tuples);
	}
	free(*c);
	*c = NULL;
}
