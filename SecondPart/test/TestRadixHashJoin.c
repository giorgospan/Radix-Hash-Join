/**
 * Unit testing on Radix Hash Join functions [Partition.c | Build.c | Probe.c]
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "CUnit/Basic.h"
#include "Intermediate.h"
#include "JobScheduler.h"
#include "Operations.h"
#include "Partition.h"
#include "Build.h"
#include "Queue.h"
#include "Probe.h"


/**
 * @brief      Tests if histogram and prefix sum are correct.
 */
void testPartition(void)
{
  RADIX_BITS         = 4;
  HASH_RANGE_1       = 16;
  initSize           = 20;

  RadixHashJoinInfo info;
	uint64_t col[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};
	info.isInInter   = 0;
	info.col         = col;
	info.tupleSize   = 1;
	info.numOfTuples = 16;
	partition(&info);

	/*
	 * HASH_FUN_1(col[i]) will be different for each col element.
	 * Thus, we expect one column element per bucket.
	 *
	 * pSum[i] will be equal to i. [1 element per bucket]
	 */
	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		CU_ASSERT_EQUAL(1,info.hist[i]);
		CU_ASSERT_EQUAL(i,info.pSum[i]);
	}

  destroyRadixHashJoinInfo(&info);
}

/**
 * @brief      Tests if column is sorted appropriately
 */
void testPartitionFunc(void)
{
  RADIX_BITS         = 4;
  HASH_RANGE_1       = 16;
  initSize           = 20;

  RadixHashJoinInfo info;
	uint64_t col[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};
	info.isInInter   = 0;
	info.col         = col;
	info.tupleSize   = 1;
	info.numOfTuples = 16;
	partition(&info);

  for(unsigned i=0;i<HASH_RANGE_1;++i)
		CU_ASSERT_EQUAL(i,info.sorted->values[i]);

  destroyRadixHashJoinInfo(&info);
}

void testBuildFunc(void)
{
  RADIX_BITS         = 4;
  HASH_RANGE_1       = 16;
  initSize           = 20;

	RadixHashJoinInfo infoLeft,infoRight;
	uint64_t col1[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};
	uint64_t col2[]  = {22u,0u,2u,0u,6u};

	infoLeft.isInInter    = 0;
	infoLeft.col          = col1;
	infoLeft.numOfTuples  = 16;
	infoLeft.tupleSize    = 1;

	infoRight.isInInter   = 0;
	infoRight.col         = col2;
	infoRight.numOfTuples = 5;
	infoRight.tupleSize   = 1;
	partition(&infoLeft);
	partition(&infoRight);

	build(&infoLeft,&infoRight);

  destroyRadixHashJoinInfo(&infoLeft);
  destroyRadixHashJoinInfo(&infoRight);
}

void testJoinFunc(void)
{
  RADIX_BITS         = 4;
  HASH_RANGE_1       = 16;
  initSize           = 20;

	RadixHashJoinInfo infoLeft,infoRight;
	uint64_t col1[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};
	uint64_t col2[]  = {22u,0u,2u,0u,6u};

	infoLeft.isInInter    = 0;
	infoLeft.col          = col1;
	infoLeft.numOfTuples  = 16;
	infoLeft.tupleSize    = 1;

	infoRight.isInInter   = 0;
	infoRight.col         = col2;
	infoRight.numOfTuples = 5;
	infoRight.tupleSize   = 1;

  // Paritioning
	partition(&infoLeft);
	partition(&infoRight);

  // Building [bucket chaining]
	build(&infoLeft,&infoRight);
  infoLeft.isLeft  = 1;
  infoRight.isLeft = 0;

  struct Vector **results;
	results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
	MALLOC_CHECK(results);

  // Split the join jobs
  jobsFinished = 0;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		createVector(results+i,infoLeft.tupleSize+infoRight.tupleSize);
    for(unsigned i=0;i<HASH_RANGE_1;++i){
  		struct joinArg *arg = js->joinJobs[i].argument;
  		arg->bucket         = i;
  		arg->left           = &infoLeft;
  		arg->right          = &infoRight;
  		arg->results        = results[i];
  		pthread_mutex_lock(&queueMtx);
  		enQueue(jobQueue,&js->joinJobs[i]);
  		pthread_cond_signal(&condNonEmpty);
  		pthread_mutex_unlock(&queueMtx);
  	}

    // Wait for all jobs to be completed
  	pthread_mutex_lock(&jobsFinishedMtx);
  	while (jobsFinished!=HASH_RANGE_1) {
  		pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
  	}
  	jobsFinished = 0;
  	pthread_cond_signal(&condJobsFinished);
  	pthread_mutex_unlock(&jobsFinishedMtx);

    // Add partial results
    unsigned ntuples = 0;
    for(unsigned i=0;i<HASH_RANGE_1;++i)
      ntuples += getVectorTuples(results[i]);

	/* If we apply join on those two columns, we get 4 <rowId1,rowId2> tuples */
  CU_ASSERT_EQUAL(4,ntuples);


  for(unsigned i=0;i<HASH_RANGE_1;++i)
    destroyVector(results+i);
  free(results);
  destroyRadixHashJoinInfo(&infoLeft);
  destroyRadixHashJoinInfo(&infoRight);
}
