#include <stdio.h>
#include <string.h> /*strcmp()*/
#include <stdlib.h> /*exit()*/
#include "Joiner.h"
#include "Parser.h"
#include "Utils.h"

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
	while (fgets(buffer, sizeof(buffer), stdin) != NULL)
	{	
		if(!strcmp(buffer,"F\n"))continue;

		createQueryInfo(&q,buffer);
		// printTest(q);
		join(joiner,q);
		destroyQueryInfo(q);
	}

    // Cleanup memory 
    destroyJoiner(joiner);
	return 0;
}