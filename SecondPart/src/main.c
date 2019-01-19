#include <stdio.h>
#include <string.h> /*strcmp()*/
#include <stdlib.h> /*exit()*/
#include <time.h>/*Debugging*/
#include <unistd.h> /*sleep()--debugging*/
#include <pthread.h>
#include "Joiner.h"
#include "Parser.h"
#include "JobScheduler.h"
#include "Optimizer.h"


int main(int argc, char const *argv[])
{
		// Create a new Joiner
		// Joiner holds meta data for every relation
		struct Joiner* joiner;
		createJoiner(&joiner);

    // Read join relations and load them to memory
    setup(joiner);

		// Create Job Scheduler[worker threads,JobQueue, e.t.c]
		createJobScheduler(&js);

    // Read query line
    // Parse query by splitting it into parts
		// Find estimations using the statistics of each column
		// Find the optimal join order
    // Execute query and write checksum to stdout
    // Destroy query
		struct QueryInfo *q;
		char buffer[BUFFERSIZE];
		while (fgets(buffer, sizeof(buffer), stdin) != NULL)
		{
			if(!strcmp(buffer,"F\n"))continue;

			createQueryInfo(&q,buffer);
			createQueryEstimations(q,joiner);

			// printColumnStats(&q->estimations[0][0]);

			applyColEqualityEstimations(q,joiner);
			applyFilterEstimations(q,joiner);
			applyJoinEstimations(q,joiner);
			// printColumnStats(&q->estimations[0][0]);
			// fprintf(stderr, "\n\n");
			// findOptimalJoinOrder(q,joiner);
			join(joiner,q);
			destroyQueryInfo(q);
		}

	  // Cleanup memory
		destroyJobScheduler(js);
		destroyJoiner(joiner);
		return 0;
}
