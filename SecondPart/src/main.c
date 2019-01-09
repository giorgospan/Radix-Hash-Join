#include <stdio.h>
#include <string.h> /*strcmp()*/
#include <stdlib.h> /*exit()*/
#include <time.h>/*Debugging*/
#include "Joiner.h"
#include "Parser.h"
#include "Partition.h"/*Setting RADIX_BITS*/
#include "Utils.h"

double start,end,duration,max;
unsigned maxQuery;

int main(int argc, char const *argv[])
{

	// Create a new Joiner
	// Joiner holds meta data for every relation
	struct Joiner* joiner;
	createJoiner(&joiner);

    // Read join relations and load them to memory
    setup(joiner);

    // Read query line
    // Parse query by splitting it into parts
    // Execute query and write checksum to stdout
    // Destroy query
	struct QueryInfo *q;
	char buffer[BUFFERSIZE];
	unsigned i=0;
	setRadixBits(joiner);
	while (fgets(buffer, sizeof(buffer), stdin) != NULL)
	{
		if(!strcmp(buffer,"F\n"))continue;

		createQueryInfo(&q,buffer);
		start = clock();
		join(joiner,q);
		end = clock();
		// printTest(q);
		// fprintf(stderr, "\nQuery[%u]:%.5lf\n\n",i++,(double)(end-start)/CLOCKS_PER_SEC);
		duration = end-start;

		if(!i){
			max      = duration;
			maxQuery = i++;
		}
		else if (duration>max){
			max      = duration;
			maxQuery = i++;
		}
		destroyQueryInfo(q);
	}
	// fprintf(stderr, "\nQuery[%u]: %.3lf sec\n\n",maxQuery,(double)max/CLOCKS_PER_SEC);
  // Cleanup memory
  destroyJoiner(joiner);
	return 0;
}
