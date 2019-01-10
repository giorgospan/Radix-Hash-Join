#include <stdio.h>
#include <stdlib.h> /*malloc(),free()*/
#include <string.h>/* strerror() */
#include <unistd.h> /*sleep()--debugging*/
#include <time.h> /*time()--debugging*/
#include <pthread.h>


#include "Utils.h" /*allocate()*/
#include "JobScheduler.h"
#include "Joiner.h"
#include "Queue.h"

pthread_mutex_t queueMtx    = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t condNonEmpty = PTHREAD_COND_INITIALIZER;
struct Queue* jobQueue      = NULL;
pthread_barrier_t barrier;


void createJobScheduler(struct JobScheduler** js){

  *js              = allocate(sizeof(struct JobScheduler),"createJobScheduler1");
  (*js)->threadNum = 4;
  (*js)->tids      = allocate((*js)->threadNum*sizeof(pthread_t),"createJobScheduler2");

  // Create Queue with 1000 jobs max size
  createQueue(&jobQueue,100);

  // Create worker threads
  int err;
  for(unsigned i=0;i<(*js)->threadNum;++i){
    if(err = pthread_create((*js)->tids+i,NULL,threadFunc,(void*)NULL)){
      fprintf(stderr, "pthread_create: %s\n",strerror(err));
      exit(EXIT_FAILURE);
    }
  }
  /* Initialize the barrier */
  pthread_barrier_init(&barrier, NULL, (*js)->threadNum+1);
  /* Create job arrays */
  createJobArrays(*js);

}

void createJobArrays(struct JobScheduler* js){

  /* Create array with  histograms */
  js->histArray = malloc(js->threadNum*sizeof(unsigned*));
  for(unsigned i=0;i<js->threadNum;++i)
    js->histArray[i] = malloc(HASH_RANGE_1*sizeof(unsigned));

  /* Create array with histogram jobs */
  js->histJobs = malloc(js->threadNum*sizeof(struct Job));
  for(unsigned i=0;i<js->threadNum;++i){
    js->histJobs[i].argument                                = malloc(sizeof(struct histArg));
    ((struct histArg *)js->histJobs[i].argument)->histogram = js->histArray[i];
    js->histJobs[i].function 	                              = histFunc;
  }

  /* Create array with partition jobs */
  js->partitionJobs = malloc(js->threadNum*sizeof(struct Job));
  for(unsigned i=0;i<js->threadNum;++i){
    js->partitionJobs[i].argument  = malloc(sizeof(struct partitionArg));
    js->partitionJobs[i].function = partitionFunc;
  }

  /* Create array with build jobs */
  js->buildJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->buildJobs[i].argument = malloc(sizeof(struct buildArg));
    js->buildJobs[i].function = buildFunc;
  }

  /* Create array with join jobs */
  js->joinJobs = malloc(HASH_RANGE_1*sizeof(struct Job));
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    js->joinJobs[i].argument = malloc(sizeof(struct joinArg));
    js->joinJobs[i].function = partitionFunc;
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
      pthread_exit(NULL);
    }

    // Execute the function of your job & destroy the argument afterwards[if it has been malloc'd]
    (*(job->function))(job->argument);
    // free(job->argument);
  }
}

void destroyJobScheduler(struct JobScheduler* js){

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
  if(err = pthread_cond_destroy(&condNonEmpty)){
    fprintf(stderr, "pthread_cond_destroy: %s\n",strerror(err));
    exit(EXIT_FAILURE);
  }

  // Destroy barrier
  pthread_barrier_destroy(&barrier);

  // Destroy arrays with jobs and histograms
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

  // Destroy JobScheduler
  free(js);
}
