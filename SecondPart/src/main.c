#include <stdio.h>
#include <string.h> /*strcmp()*/
#include <stdlib.h> /*exit()*/
#include <time.h>/*Debugging*/
#include <unistd.h> /*sleep()--debugging*/
#include <pthread.h>
#include "Joiner.h"
#include "Parser.h"
#include "Partition.h"/*Setting RADIX_BITS*/
#include "JobScheduler.h"
#include "Queue.h"
#include "Utils.h"


int main(int argc, char const *argv[])
{
	  double elapsed;
	  struct timespec start, finish;

		// Create a new Joiner
		// Joiner holds meta data for every relation
		struct Joiner* joiner;
		createJoiner(&joiner);

    // Read join relations and load them to memory
    setup(joiner);

		// Create Job Scheduler[worker threads,JobQueue, e.t.c]
		struct JobScheduler* js;
		createJobScheduler(&js);

    // Read query line
    // Parse query by splitting it into parts
    // Execute query and write checksum to stdout
    // Destroy query
		struct QueryInfo *q;
		char buffer[BUFFERSIZE];
		while (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			if(!strcmp(buffer,"F\n"))continue;

			createQueryInfo(&q,buffer);
			join(joiner,q);
			destroyQueryInfo(q);
		}

		clock_gettime(CLOCK_MONOTONIC, &start);
		for (unsigned times = 0; times < 1; times++) {
			/* Initialize our dummy column */
			for(unsigned i=0;i<N;++i){
				column[i] = rand() % 1000000;
			}

			// Send a bunch of histogram jobs
			unsigned chunkSize = N/js->threadNum;
			for(unsigned i=0;i<js->threadNum;++i){
				struct histArg* pArg       = js->histJobs[i].argument;
				pArg->start		 	           = i*chunkSize;
				pArg->end 		             = pArg->start+chunkSize;
				pthread_mutex_lock(&queueMtx);
				enQueue(jobQueue,&js->histJobs[i]);
				pthread_cond_signal(&condNonEmpty);
				pthread_mutex_unlock(&queueMtx);
			}
			// Wait for all threads to compute their histograms
			pthread_barrier_wait(&barrier);
			// for(unsigned i=0;i<js->threadNum;++i){
			// 		unsigned sum = 0;
			// 		for(unsigned b=0;b<HASH_RANGE_1;++b)
			// 			sum+= js->histArray[i][b];
			// 		if(sum!=chunkSize){
			// 			fprintf(stderr, "============================\n");
			// 			fprintf(stderr, "Thread[%u]\n",i);
			// 			fprintf(stderr, "Sum:%u [Error]\n",sum);
			// 			fprintf(stderr, "============================\n");
			// 		}
			// 	}
		}

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


	  // Cleanup memory
		destroyJobScheduler(js);
		// Calculate execution time
		clock_gettime(CLOCK_MONOTONIC, &finish);
		elapsed = (finish.tv_sec - start.tv_sec);
		elapsed += (finish.tv_nsec - start.tv_nsec) / 1000000000.0;
		fprintf(stderr, "Duration[MainThread]:%.5lf seconds\n",elapsed);
		fprintf(stderr, "==========================================================\n");
		destroyJoiner(joiner);
		return 0;
}
