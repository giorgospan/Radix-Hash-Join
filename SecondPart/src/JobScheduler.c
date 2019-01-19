#include <stdio.h>
#include <stdlib.h> /*malloc(),free()*/
#include <string.h>/* strerror() */
#include <unistd.h> /*sleep()--debugging*/
#include <time.h> /*time()--debugging*/
#include <pthread.h>

#include "Utils.h"
#include "JobScheduler.h"
#include "Joiner.h"
#include "Partition.h"
#include "Build.h"
#include "Probe.h"
#include "Operations.h"
#include "Queue.h"

#define THREAD_NUM 8

pthread_mutex_t queueMtx           = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t jobsFinishedMtx    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condNonEmpty        = PTHREAD_COND_INITIALIZER;
pthread_cond_t condJobsFinished    = PTHREAD_COND_INITIALIZER;
struct Queue* jobQueue             = NULL;
struct JobScheduler* js            = NULL;
pthread_mutex_t* partitionMtxArray = NULL;
unsigned jobsFinished              = 0;
pthread_barrier_t barrier;

void createJobScheduler(struct JobScheduler** js){

  *js              = malloc(sizeof(struct JobScheduler));
  MALLOC_CHECK(*js);
  (*js)->threadNum = THREAD_NUM;
  (*js)->tids      = malloc((*js)->threadNum*sizeof(pthread_t));
  MALLOC_CHECK((*js)->tids);

  // Create Queue with 1000 jobs max size
  createQueue(&jobQueue,1000);

  // Create worker threads
  int err;
  for(unsigned i=0;i<(*js)->threadNum;++i){
    if(err = pthread_create((*js)->tids+i,NULL,threadFunc,(void*)0)){
      exit(EXIT_FAILURE);
    }
  }

  /* Initialize partition mutex array */
  partitionMtxArray = malloc(HASH_RANGE_1*sizeof(pthread_mutex_t));
  MALLOC_CHECK(partitionMtxArray);
  for(unsigned i=0;i<HASH_RANGE_1;++i)
    pthread_mutex_init(partitionMtxArray+i,NULL);

  /* Initialize the barrier */
  pthread_barrier_init(&barrier, NULL, (*js)->threadNum+1);
  /* Create job arrays */
  createJobArrays(*js);
}

void createJobArrays(struct JobScheduler* js){

  /* Create array with checkSums */
  js->checkSumArray = malloc(HASH_RANGE_1*sizeof(uint64_t));
  MALLOC_CHECK(js->checkSumArray);

  /* Create array with  histograms */
  js->histArray = malloc(js->threadNum*sizeof(unsigned*));
  MALLOC_CHECK(js->histArray);
  for(unsigned i=0;i<js->threadNum;++i)
  {
    js->histArray[i] = malloc(HASH_RANGE_1*sizeof(unsigned));
    MALLOC_CHECK(js->histArray[i]);
  }

  /* Create array with histogram jobs */
  js->histJobs = malloc(js->threadNum*sizeof(struct Job));
  MALLOC_CHECK(js->histJobs);
  for(unsigned i=0;i<js->threadNum;++i){
    js->histJobs[i].argument                                = malloc(sizeof(struct histArg));
    MALLOC_CHECK(js->histJobs[i].argument);
    ((struct histArg *)js->histJobs[i].argument)->histogram = js->histArray[i];
    js->histJobs[i].function 	                              = histFunc;
  }

  /* Create array with partition jobs */
  js->partitionJobs = malloc(js->threadNum*sizeof(struct Job));
  MALLOC_CHECK(js->partitionJobs);
  for(unsigned i=0;i<js->threadNum;++i){
    js->partitionJobs[i].argument  = malloc(sizeof(struct partitionArg));
    MALLOC_CHECK(js->partitionJobs[i].argument);
    js->partitionJobs[i].function = partitionFunc;
  }

  /* Create array with build jobs */
  js->buildJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  MALLOC_CHECK(js->buildJobs);
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->buildJobs[i].argument = malloc(sizeof(struct buildArg));
    MALLOC_CHECK(js->buildJobs[i].argument);
    js->buildJobs[i].function = buildFunc;
  }

  /* Create array with join jobs */
  js->joinJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  MALLOC_CHECK(js->joinJobs);
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->joinJobs[i].argument = malloc(sizeof(struct joinArg));
    MALLOC_CHECK(js->joinJobs[i].argument);
    js->joinJobs[i].function = joinFunc;
  }

  /* Create array with columnEquality jobs */
  js->colEqualityJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  MALLOC_CHECK(js->colEqualityJobs);
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->colEqualityJobs[i].argument = malloc(sizeof(struct colEqualityArg));
    MALLOC_CHECK(js->colEqualityJobs[i].argument);
    js->colEqualityJobs[i].function = colEqualityFunc;
  }

  /* Create array with filter jobs */
  js->filterJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  MALLOC_CHECK(js->filterJobs);
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->filterJobs[i].argument = malloc(sizeof(struct filterArg));
    MALLOC_CHECK(js->filterJobs[i].argument);
    js->filterJobs[i].function = filterFunc;
  }

  /* Create array with checksum jobs */
  js->checkSumJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  MALLOC_CHECK(js->checkSumJobs);
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->checkSumJobs[i].argument = malloc(sizeof(struct checkSumArg));
    MALLOC_CHECK(js->checkSumJobs[i].argument);
    js->checkSumJobs[i].function = checkSumFunc;
  }
}

void *threadFunc(void * arg){

  // fprintf(stderr, "thread[%u] entering the threadFunc\n",(unsigned)pthread_self());
  int err;
  while(1){
    // Acquire mutex and check if there is a job in the queue
    pthread_mutex_lock(&queueMtx);
    while (isEmpty(jobQueue)) {
      // fprintf(stderr, "Going to sleep\n");
      pthread_cond_wait(&condNonEmpty,&queueMtx);
      // fprintf(stderr, "thread[%u] woke up\n",(unsigned)pthread_self());
    }

    // Finally found a job to work on
    // deQueue the job and release the mutex [also signal the cond var]
    struct Job* job = deQueue(jobQueue);
    pthread_cond_signal(&condNonEmpty);
    pthread_mutex_unlock(&queueMtx);

    // Special job indicating the end of our program
    if(job==NULL)
    {
      // fprintf(stderr, "thread[%u] exiting...\n",(unsigned)pthread_self());
      pthread_exit((void*)0);
    }

    // Execute the function of your job & destroy the argument afterwards[if it has been malloc'd]
    (*(job->function))(job->argument);
  }
}

void destroyJobScheduler(struct JobScheduler* js){

  // Send "termination" jobs
  for(unsigned i=0;i<js->threadNum;++i){
    pthread_mutex_lock(&queueMtx);
    enQueue(jobQueue,NULL);
    pthread_cond_signal(&condNonEmpty);
    pthread_mutex_unlock(&queueMtx);
  }
  // Broadcast to make sure every worker gets its "termination" job
  pthread_mutex_lock(&queueMtx);
  pthread_cond_broadcast(&condNonEmpty);
  pthread_mutex_unlock(&queueMtx);

  // Join worker threads
  int err;
  for(unsigned i=0;i<js->threadNum;++i){
    if(err = pthread_join(js->tids[i],NULL)){
      fprintf(stderr, "pthread_join: %s\n",strerror(err));
      exit(EXIT_FAILURE);
    }
  }

  // Destroy thread id table
  free(js->tids);

  // Destroy Queue
  destroyQueue(jobQueue);

  // Destroy mutexes and cond vars
  if (err = pthread_mutex_destroy(&queueMtx)) {
    fprintf(stderr, "pthread_mutex_destroy: %s\n",strerror(err));
    exit(EXIT_FAILURE);
  }

  if (err = pthread_mutex_destroy(&jobsFinishedMtx)) {
    fprintf(stderr, "pthread_mutex_destroy: %s\n",strerror(err));
    exit(EXIT_FAILURE);
  }

  if(err = pthread_cond_destroy(&condNonEmpty)){
    fprintf(stderr, "pthread_cond_destroy: %s\n",strerror(err));
    exit(EXIT_FAILURE);
  }

  if(err = pthread_cond_destroy(&condJobsFinished)){
    fprintf(stderr, "pthread_cond_destroy: %s\n",strerror(err));
    exit(EXIT_FAILURE);
  }

  // Destroy partition mutex array
  for(unsigned i=0;i<HASH_RANGE_1;++i)
  if (err = pthread_mutex_destroy(partitionMtxArray+i)) {
    fprintf(stderr, "pthread_mutex_destroy: %s\n",strerror(err));
    exit(EXIT_FAILURE);
  }
  free(partitionMtxArray);

  // Destroy barrier
  pthread_barrier_destroy(&barrier);

  // Destroy arrays with jobs , checkSums and histograms
  free(js->checkSumArray);

  for(unsigned i=0;i<js->threadNum;++i)
    free(js->histArray[i]);
  free(js->histArray);

  for(unsigned i=0;i<js->threadNum;++i)
    free(js->histJobs[i].argument);
  free(js->histJobs);

  for(unsigned i=0;i<js->threadNum;++i)
    free(js->partitionJobs[i].argument);
  free(js->partitionJobs);

  for(unsigned i=0;i<HASH_RANGE_1;++i)
    free(js->buildJobs[i].argument);
  free(js->buildJobs);

  for(unsigned i=0;i<HASH_RANGE_1;++i)
    free(js->joinJobs[i].argument);
  free(js->joinJobs);

  for(unsigned i=0;i<HASH_RANGE_1;++i)
    free(js->colEqualityJobs[i].argument);
  free(js->colEqualityJobs);

  for(unsigned i=0;i<HASH_RANGE_1;++i)
    free(js->filterJobs[i].argument);
  free(js->filterJobs);


  for(unsigned i=0;i<HASH_RANGE_1;++i)
    free(js->checkSumJobs[i].argument);
  free(js->checkSumJobs);

  // Destroy JobScheduler
  free(js);
}
