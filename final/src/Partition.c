#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <string.h>/* strerror() */
#include <errno.h>/* errno */
#include <pthread.h>

#include "Partition.h"
#include "Intermediate.h"
#include "JobScheduler.h"
#include "Queue.h"
#include "Utils.h"
#include "Vector.h"

#define PARALLEL_PARTITION 1
#define PARALLEL_HISTOGRAM 1

void histFunc(void* arg)
{
  // double elapsed;
  // struct timespec start, finish;
  // clock_gettime(CLOCK_MONOTONIC, &start);

	struct histArg *myarg = arg;
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    myarg->histogram[i] = 0;
  }
  for(unsigned i=myarg->start;i<myarg->end;++i){
    myarg->histogram[HASH_FUN_1(myarg->values[i])] += 1;
  }
  // clock_gettime(CLOCK_MONOTONIC, &finish);
  // elapsed = (finish.tv_sec - start.tv_sec);
  // elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  // fprintf(stderr, "Duration[%u]:%.5lf seconds\n",(unsigned)pthread_self(),elapsed);

  pthread_barrier_wait(&barrier);
}

void partitionFunc(void* arg)
{
  // double elapsed;
  // struct timespec start, finish;
  // clock_gettime(CLOCK_MONOTONIC, &start);

	struct partitionArg *myarg = arg;
	for(unsigned i=myarg->start;i<myarg->end;++i)
	{
		/* Get the hashValue from unsorted column */
		unsigned h;
		if(myarg->info->isInInter)
      h = HASH_FUN_1(myarg->info->unsorted->values[i]);
		else
      h = HASH_FUN_1(myarg->info->col[i]);

		/* Go to pSumCopy and find where we need to place it in sorted column  */
		unsigned offset;
    pthread_mutex_lock(&partitionMtxArray[h]);
		offset = myarg->pSumCopy[h];


		/**
		* Increment the value to know where to put he next element with the same hashValue
		*/
		myarg->pSumCopy[h]++;
		if(myarg->info->isInInter)
		{
			myarg->info->sorted->values[offset] = myarg->info->unsorted->values[i];
			myarg->info->sorted->rowIds[offset] = myarg->info->unsorted->rowIds[i];
			insertAtPos(myarg->info->sorted->tuples,&myarg->info->unsorted->tuples->table[i*myarg->info->tupleSize],offset);
		}
		else
		{
			myarg->info->sorted->values[offset] = myarg->info->col[i];
			myarg->info->sorted->rowIds[offset] = i;
			insertAtPos(myarg->info->sorted->tuples,&i,offset);
		}
    pthread_mutex_unlock(&partitionMtxArray[h]);
	}
  // clock_gettime(CLOCK_MONOTONIC, &finish);
  // elapsed = (finish.tv_sec - start.tv_sec);
  // elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  // fprintf(stderr, "Duration[%u]:%.5lf seconds\n",(unsigned)pthread_self(),elapsed);
	pthread_barrier_wait(&barrier);
}

void partition(RadixHashJoinInfo *info)
{
	info->unsorted         = malloc(sizeof(ColumnInfo));
  MALLOC_CHECK(info->unsorted);

  info->unsorted->values = malloc(info->numOfTuples*sizeof(uint64_t));
  MALLOC_CHECK(info->unsorted->values);

  info->unsorted->rowIds = malloc(info->numOfTuples*sizeof(unsigned));
  MALLOC_CHECK(info->unsorted->rowIds);

  info->sorted           = malloc(sizeof(ColumnInfo));
  MALLOC_CHECK(info->sorted);

  info->sorted->values   = malloc(info->numOfTuples*sizeof(uint64_t));
  MALLOC_CHECK(info->sorted->values);

  info->sorted->rowIds   = malloc(info->numOfTuples*sizeof(unsigned));
  MALLOC_CHECK(info->sorted->rowIds);

  info->indexArray       = NULL;

  createVectorFixedSize(&info->sorted->tuples,info->tupleSize,info->numOfTuples);

	createVector(&info->unsorted->tuples,info->tupleSize);

	// Get values-rowIds from intermediate vector
	if(info->isInInter)
		scanJoin(info);

	// Create our main histogram
	info->hist = malloc(HASH_RANGE_1*sizeof(unsigned));
  MALLOC_CHECK(info->hist);

#if PARALLEL_HISTOGRAM
	// Enqueue histogram jobs
	unsigned chunkSize = info->numOfTuples / js->threadNum;
	unsigned lastEnd = 0;
	unsigned i;
	for(i=0;i<js->threadNum-1;++i)
	{
		struct histArg *arg = js->histJobs[i].argument;
		arg->start          = i*chunkSize;
		arg->end            = arg->start + chunkSize;
		arg->values         = (info->isInInter) ? info->unsorted->values : info->col;
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->histJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
		lastEnd = arg->end;
	}
	// Remaining values, in case numOfTuples could not be divided exactly between the threads
	struct histArg *arg = js->histJobs[i].argument;
	arg->start          = lastEnd;
	arg->end            = info->numOfTuples;
	arg->values         = (info->isInInter) ? info->unsorted->values : info->col;
	pthread_mutex_lock(&queueMtx);
	enQueue(jobQueue,&js->histJobs[i]);
	pthread_cond_signal(&condNonEmpty);
	pthread_mutex_unlock(&queueMtx);

	// Wait until all threads are done
  // fprintf(stderr, "%s\n","Before barrier" );
	pthread_barrier_wait(&barrier);
  // fprintf(stderr, "%s\n","After barrier" );


	// Merge the partial histograms
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		info->hist[i] = 0;

	for(unsigned t=0;t<js->threadNum;++t)
		for(unsigned h=0;h<HASH_RANGE_1;++h)
			info->hist[h]+=js->histArray[t][h];
#else

  for(unsigned i=0;i<HASH_RANGE_1;++i)
    info->hist[i] = 0;

  for(unsigned i=0;i<info->numOfTuples;++i)
    if(info->isInInter)
    	info->hist[HASH_FUN_1(info->unsorted->values[i])] += 1;
    else
    	info->hist[HASH_FUN_1(info->col[i])] += 1;
#endif

  // Calculate Prefix Sum
	unsigned sum = 0;
	info->pSum   = malloc(HASH_RANGE_1*sizeof(unsigned));
  MALLOC_CHECK(info->pSum);

	for(unsigned i=0;i<HASH_RANGE_1;++i)
		info->pSum[i] = 0;

	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		info->pSum[i] = sum;
		sum += info->hist[i];
	}

  unsigned *pSumCopy   = malloc(HASH_RANGE_1*sizeof(unsigned));
  MALLOC_CHECK(pSumCopy);
  for(unsigned i=0;i<HASH_RANGE_1;++i)
    pSumCopy[i] = info->pSum[i];

#if PARALLEL_PARTITION
	lastEnd = 0;
	for(i=0;i<js->threadNum-1;++i)
	{
		struct partitionArg *arg = js->partitionJobs[i].argument;
		arg->start               = i*chunkSize;
		arg->end                 = arg->start + chunkSize;
    arg->pSumCopy            = pSumCopy;
    arg->info                = info;
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->partitionJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
		lastEnd = arg->end;
	}
	// Remaining values, in case numOfTuples could not be divided exactly between the threads
  struct partitionArg *pArg = js->partitionJobs[i].argument;
  pArg->start               = lastEnd;
	pArg->end                 = info->numOfTuples;
  pArg->pSumCopy            = pSumCopy;
  pArg->info                = info;
	pthread_mutex_lock(&queueMtx);
	enQueue(jobQueue,&js->partitionJobs[i]);
	pthread_cond_signal(&condNonEmpty);
	pthread_mutex_unlock(&queueMtx);

	// Wait until all threads are done
  // fprintf(stderr, "%s\n","Before barrier" );
	pthread_barrier_wait(&barrier);
  // fprintf(stderr, "%s\n","After barrier" );
#else

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
#endif
  free(pSumCopy);
}
