#include <stdio.h>
#include <stdlib.h> /*free()*/

#include "Probe.h"
#include "Vector.h"
#include "Build.h"
#include "Utils.h"

void probe(RadixHashJoinInfo *left,RadixHashJoinInfo *right,struct Vector *results)
{
	unsigned i;
	unsigned searchBucket;
	unsigned searchValue;
	uint64_t hash;
	unsigned start;
	int k;
	struct Index *searchIndex;
	left->isLeft  = 1;
	right->isLeft = 0;

	RadixHashJoinInfo *big,*small;
	big   = left->isSmall ? right:left;
	small = left->isSmall ? left:right;

	/* For every tuple(i.e:record) in the big relation */
	for(i=0;i<big->columnSize;i++)
	{
		/* The value we are gonna search for */
		searchValue = big->sorted->values[i];

		/* Find out in which bucket of the small relation
			we should search */
		searchBucket = HASH_FUN_1(searchValue);

		/* Bucket is empty, there is nothing to search here */
		if(small->hist[searchBucket] == 0)
			continue;

		/* Fetch starting point of the bucket */
		start = small->pSum[searchBucket];

		/* Fetch the index of this bucket */
		searchIndex = small->indexArray[searchBucket];

		/* Find out where to look for in the bucketArray of the index */
		hash = HASH_FUN_2(searchValue);

		/* Bucket is not empty, but there is no value equal to the searchValue */
		if(searchIndex->bucketArray[hash]==0)
			continue;

		/* Warning: In bucketArray and chainArray we've stored the rowIds relevant to the bucket [i.e: 0 ~> bucketSize-1] */

		k = searchIndex->bucketArray[hash] - 1;
		checkEqual(small,big,i,start,searchValue,k,results);

		while(1)
		{	
			// We've reached the end of the chain
			if(searchIndex->chainArray[k] == 0)
				break;

			/* Step further on the chain */
			else
			{
				k = searchIndex->chainArray[k] - 1;
				checkEqual(small,big,i,start,searchValue,k,results);
			}
		}

	}
}

void checkEqual(RadixHashJoinInfo *small,RadixHashJoinInfo *big,unsigned i,unsigned start,unsigned searchValue,unsigned pseudoRow,struct Vector *results)
{
	uint32_t actualRow;
	unsigned tupleSize      = small->tupleSize+big->tupleSize;
	unsigned *tupleToInsert = allocate(tupleSize*sizeof(unsigned),"checkEqual");

	/* We calculate the rowId relevant to the sorted array [i.e: 0 ~> columnSize] */
	actualRow = start + pseudoRow;
	
	/*
	 * Check equality and insert tuple into results vector
	 * The new tuple is constructed by combining the tuple of each relation. 
	 */
	if(small->sorted->values[actualRow] == searchValue)
	{
		constructTuple(small,big,actualRow,i,tupleToInsert);
		insertAtVector(results,tupleToInsert);
	}
	free(tupleToInsert);
}


void constructTuple(RadixHashJoinInfo *small,RadixHashJoinInfo *big,unsigned actualRow,unsigned i,unsigned *target)
{
	/**
	 * We need to construct the tuple we're gonna add in the results vector
	 * 
	 * First,we add left column's tuple [it is important to add left column's tuple first]
	 * Then,we add right column's tuple.
	 * 
	 * Notice that we need to access different row depending on 
	 * whether the column is small or big [i.e: indexed or not]
	 */
	unsigned *t;
	unsigned k=0;
	RadixHashJoinInfo *left  = getLeftCol(small,big);
	RadixHashJoinInfo *right = getRightCol(small,big);

	// Add values from left column's tuple
	if(left->isSmall)
		t = getTuple(left->sorted->tuples,actualRow);
	else
		t = getTuple(left->sorted->tuples,i);

	for(unsigned i=0;i<left->tupleSize;++i)
		target[k++] = t[i];

	// Add values from right column's tuple
	if(right->isSmall)
		t = getTuple(right->sorted->tuples,actualRow);
	else
		t = getTuple(right->sorted->tuples,i);

	for(unsigned i=0;i<right->tupleSize;++i)
		target[k++] = t[i];
}

RadixHashJoinInfo *getLeftCol(RadixHashJoinInfo *i1,RadixHashJoinInfo *i2)
{
	return i1->isLeft ? i1 : i2;
}

RadixHashJoinInfo *getRightCol(RadixHashJoinInfo *i1,RadixHashJoinInfo *i2)
{
	return i1->isLeft ? i2 : i1;
}