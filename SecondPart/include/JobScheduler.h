#ifndef JOB_SCHEDULER_H
#define JOB_SCHEDULER_H

/* Mutexes - conditional variables - barriers */
extern pthread_mutex_t queueMtx;
extern pthread_cond_t condNonEmpty;
extern pthread_barrier_t barrier;
extern struct JobScheduler* js;

/* Job queue */
/* It must be visible from all threads including the main thread of course */
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
    // job arrays [4 kinds of jobs]
    struct Job *histJobs;
    struct Job *partitionJobs;
    struct Job *buildJobs;
    struct Job *joinJobs;
};

void createJobScheduler(struct JobScheduler** js);
void createJobArrays(struct JobScheduler* js);
void *threadFunc(void *);
void destroyJobScheduler(struct JobScheduler* js);

#endif
