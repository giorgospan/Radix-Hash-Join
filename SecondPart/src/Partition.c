#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>/* strerror() */
#include <errno.h>/* errno */

#include "Partition.h"
#include "Intermediate.h"
#include "Utils.h"
#include "Vector.h"


void histFunc(void* arg)
{
	// struct histArg *myarg = arg;
  // double elapsed;
  // struct timespec start, finish;
  // clock_gettime(CLOCK_MONOTONIC, &start);
  // for(unsigned i=0;i<HASH_RANGE_1;++i){
  //   myarg->histogram[i] = 0;
  // }
  // for(unsigned i=myarg->start;i<myarg->end;++i){
  //   myarg->histogram[HASH_FUN_1(column[i])] += 1;
  // }
  // clock_gettime(CLOCK_MONOTONIC, &finish);
  // elapsed = (finish.tv_sec - start.tv_sec);
  // elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  // fprintf(stderr, "Duration[%u]:%.5lf seconds\n",(unsigned)pthread_self(),elapsed);
  //
  // pthread_barrier_wait(&barrier);

}

void partitionFunc(void* arg)
{

}


void partition(RadixHashJoinInfo *info)
{
	info->unsorted          = allocate(sizeof(ColumnInfo),"partition2");
	info->unsorted->values  = allocate(info->numOfTuples*sizeof(uint64_t),"partition3");
	info->unsorted->rowIds  = allocate(info->numOfTuples*sizeof(unsigned),"partition4");
	info->indexArray        = NULL;

	createVector(&info->unsorted->tuples,info->tupleSize);

	// Get values-rowIds from intermediate vector
	if(info->isInInter)
		scanJoin(info);

	// Create Histogram
	info->hist = allocate(HASH_RANGE_1*sizeof(unsigned),"partition5");

	for(unsigned i=0;i<HASH_RANGE_1;++i)
		info->hist[i] = 0;

	for(unsigned i=0;i<info->numOfTuples;++i)
		if(info->isInInter)
			info->hist[HASH_FUN_1(info->unsorted->values[i])] += 1;
		else
			info->hist[HASH_FUN_1(info->col[i])] += 1;

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
}

void sortColumn(RadixHashJoinInfo *info)
{
	info->sorted         = allocate(sizeof(ColumnInfo),"sortColumn1");
	info->sorted->values = allocate(info->numOfTuples*sizeof(uint64_t),"sortColumn2");
	info->sorted->rowIds = allocate(info->numOfTuples*sizeof(unsigned),"sortColumn3");
	unsigned *pSumCopy   = allocate(HASH_RANGE_1*sizeof(unsigned),"sortColumn4");
	memcpy(pSumCopy,info->pSum,HASH_RANGE_1*sizeof(unsigned));

	createVectorFixedSize(&info->sorted->tuples,info->tupleSize,info->numOfTuples);

	for(unsigned i=0;i<info->numOfTuples;++i)
	{
		/* Get the hashValue from unsorted column */
		unsigned h;
		if(info->isInInter)
			h = HASH_FUN_1(info->unsorted->values[i]);
		else
			h = HASH_FUN_1(info->col[i]);

		/* Go to pSumCopy and find where we need to place it in sorted column  */
		unsigned offset;
		offset = pSumCopy[h];

		/**
		 * Increment the value to know where to put he next element with the same hashValue
		 * With this we lose the original representation of pSum
		 * If we want to have access we must create a acopy before entering this for loop
		 */
		pSumCopy[h]++;
		if(info->isInInter)
		{
			info->sorted->values[offset] = info->unsorted->values[i];
			info->sorted->rowIds[offset] = info->unsorted->rowIds[i];
			insertAtPos(info->sorted->tuples,&info->unsorted->tuples->table[i*info->tupleSize],offset);
		}
		else
		{
			info->sorted->values[offset] = info->col[i];
			info->sorted->rowIds[offset] = i;
			insertAtPos(info->sorted->tuples,&i,offset);
		}
	}
	free(pSumCopy);
}
