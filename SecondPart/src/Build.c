#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> /*sleep()--debugging*/

#include "Build.h"
#include "Partition.h"
#include "Utils.h"

void build(RadixHashJoinInfo *infoLeft,RadixHashJoinInfo *infoRight)
{
	RadixHashJoinInfo *big,*small;
	big            = (infoLeft->numOfTuples > infoRight->numOfTuples) ? infoLeft:infoRight;
	small          = (infoLeft->numOfTuples < infoRight->numOfTuples) ? infoLeft:infoRight;
	big->isSmall   = 0;
	small->isSmall = 1;

	initializeIndexArray(small);
	buildIndexPerBucket(small);
}

void initializeIndexArray(RadixHashJoinInfo *info)
{
	unsigned i,j;
	unsigned bucketSize;

	/* Firstly, we need to allocate space for indexArray */
	/* Remember: One struct Index per bucket */
	info->indexArray = allocate(HASH_RANGE_1*sizeof(struct Index*),"initializeIndexArray1");

	// For every bucket
	for(i=0;i<HASH_RANGE_1;i++)
	{
		// If bucket is empty, its index will be NULL
		if(info->hist[i] == 0)
			info->indexArray[i] = NULL;

		else
		{
			// Fetch bucket's size from hist array
			// Remember: hist is array with ints
			bucketSize = info->hist[i];

			/* Allocate space for bucket's index */
			info->indexArray[i] = allocate(sizeof(struct Index),"initializeIndexArray2");


			/* Allocate space for index's fields */
 			info->indexArray[i]->chainArray  = allocate(bucketSize*sizeof(unsigned),"initializeIndexArray3");
			info->indexArray[i]->bucketArray = allocate(HASH_RANGE_2*sizeof(unsigned),"initializeIndexArray4");


			/* Initialize chainArray and bucketArray with 0's */
			for(j=0;j<bucketSize;j++)
				info->indexArray[i]->chainArray[j] = 0;

			for(j=0;j<HASH_RANGE_2;j++)
				info->indexArray[i]->bucketArray[j] = 0;
		}
	}
}

void buildIndexPerBucket(RadixHashJoinInfo *info)
{
	unsigned i;
	unsigned bucketSize;
	uint64_t hash;
	unsigned chainPos;
	int j;
	int start;

	// For every bucket
	for(i=0;i<HASH_RANGE_1;i++)
	{
		/* If bucket is not empty[i.e: has an index] */
		if(info->indexArray[i] != NULL)
		{

			// Fetch bucket's starting point from pSum array
			// Remember: pSum is array with pointers to int
			start = info->pSum[i];

			// Fetch bucket's size from hist array
			// Remember: hist is array with ints
			bucketSize = info->hist[i];

			// fprintf(stderr,"[%u]bucketSize:%u\n",i,bucketSize);

			/* Scan from the bottom of the bucket to the top */
			for(j=start+bucketSize-1;j>=start;j--)
			{
				hash = HASH_FUN_2(info->sorted->values[j]);
				// fprintf(stderr,"\nsecondHash(%lu): %lu\n",info->sorted->values[j],hash);

				if(info->indexArray[i]->bucketArray[hash] == 0)
				{
					// fprintf(stderr,"Found empty spot in bucketArray\n");
					info->indexArray[i]->bucketArray[hash] = (j-start)+1;
					// fprintf(stderr,"bucketArray[%lu]: %u\n",hash,  info->indexArray[i]->bucketArray[hash] );
				}
				else
				{
					/* Find the first zero in chainArray
						by following the chain and
						store "(j-start) + 1" in that place */

					chainPos = info->indexArray[i]->bucketArray[hash]-1;
					traverseChain(chainPos, info->indexArray[i]->chainArray, j-start + 1);
				}
			}
		}
		// else
		// 	fprintf(stderr,"Empty bucket\n");
		// fprintf(stderr,"==============================================\n");
	}
}

void traverseChain(unsigned chainPos,unsigned* chainArray,unsigned posToBeStored)
{
	// fprintf(stderr,"Moving to chainArray[%u](now is equal to %u)\n",chainPos,chainArray[chainPos]);
	while(1)
	{
		// We've found an empty spot in chainArray
		if(chainArray[chainPos] == 0)
		{
			chainArray[chainPos] = posToBeStored;
			// fprintf(stderr,"Found empty spot on chainArray[%u]\n",chainPos);
			break;
		}
		/* Step further on the chain */
		else
		{
			chainPos = chainArray[chainPos] - 1;
			// fprintf(stderr,"Moving to chainArray[%u](now is equal to %u)\n",chainPos,chainArray[chainPos]);
			// sleep(1);
		}
	}
}
