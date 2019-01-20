#ifndef JOB_SCHEDULER_H
#define JOB_SCHEDULER_H

#include <pthread.h>

/* Mutexes - conditional variables - barriers */
extern pthread_mutex_t queueMtx;
extern pthread_mutex_t jobsFinishedMtx;
extern pthread_cond_t condNonEmpty;
extern pthread_cond_t condJobsFinished;
extern pthread_barrier_t barrier;
extern struct JobScheduler* js;
extern unsigned jobsFinished;
extern pthread_mutex_t* partitionMtxArray;


/* Job queue */
/* It must be visible from all threads, including the main thread of course */
extern struct Queue* jobQueue;

struct Job{
  // Function that the worker thread is going to execute
  void (*function)(void*);
  // Argument passed to the function
  void *argument;
};

struct JobScheduler{
    // number of worker threads
    unsigned threadNum;
    //thread ids
    pthread_t *tids;
    // histgrams
    unsigned **histArray;
    // checksums
    uint64_t *checkSumArray;

    // job arrays [different kinds of jobs]
    struct Job *histJobs;
    struct Job *partitionJobs;
    struct Job *buildJobs;
    struct Job *joinJobs;
    struct Job *colEqualityJobs;
    struct Job *filterJobs;
    struct Job *checkSumJobs;
};

void createJobScheduler(struct JobScheduler** js);
void createJobArrays(struct JobScheduler* js);
void *threadFunc(void *);
void destroyJobScheduler(struct JobScheduler* js);

#endif
