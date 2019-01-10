#include <stdio.h>
#include <unistd.h> /*sleep()--debugging*/
#include <time.h> /*time()--debugging*/
#include <pthread.h>


#include "Job.h"
#include "JobScheduler.h"

void histFunc(void* arg){

  struct histArg *myarg = arg;
  double elapsed;
  struct timespec start, finish;
  clock_gettime(CLOCK_MONOTONIC, &start);
  for(unsigned i=0;i<HASH_RANGE_1;++i){
    myarg->histogram[i] = 0;
  }
  for(unsigned i=myarg->start;i<myarg->end;++i){
    myarg->histogram[HASH_FUN_1(column[i])] += 1;
  }
  clock_gettime(CLOCK_MONOTONIC, &finish);
  elapsed = (finish.tv_sec - start.tv_sec);
  elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
  fprintf(stderr, "Duration[%u]:%.5lf seconds\n",(unsigned)pthread_self(),elapsed);

  pthread_barrier_wait(&barrier);

}

void partitionFunc(void* arg){

}

void buildFunc(void* arg){

}

void joinFunc(void* arg){

}
