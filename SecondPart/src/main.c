#include <stdio.h>
#include <string.h>
#include "Joiner.h"
#include "Parser.h"

int main(int argc, char const *argv[])
{
	// Create a new Joiner
	// Joiner holds meta data for every relation
	struct Joiner* joiner;
	createJoiner(&joiner);

    // Read join relations and load them to memory
    setup(joiner);

    // Read each query line 
    // Parse query by splitting it into parts [createQueryInfo(...)]
    // Execute query and write checksum to stdout [join(...)]
	
	// struct QueryInfo *q;
	// char buffer[1024];
	// while (fgets(buffer, sizeof(buffer), stdin) != NULL)
	// {
	// 	// End of batch
	// 	if(!strcmp(buffer,"F"))
	// 		continue;
	// 	createQueryInfo(&q,buffer);
	// 	join(joiner,q);
	// }

    // Cleanup memory 
    destroyJoiner(joiner);
	return 0;
}