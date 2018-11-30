#ifndef JOINER_H
#define JOINER_H

#include <stdint.h>
#include "Relation.h"


struct Joiner
{
	struct Relation **relations;
	uint64_t numOfRelations;
	uint64_t capacity; // Used for realloc
	
	// We might need to add some more fields later
};

/**
 * @brief      Creates a new joiner.Also, it initializes
 *             joiner's fields (relations,numOfRelations,capacity)
 *
 */
void createJoiner(struct Joiner **joiner);

/**
 * @brief      Reads filesnames[containing relations to be joined] 
 */
void setup(struct Joiner *joiner);


/**
 * @brief      Adds a new relation to the given joiner struct
 * 
 * @param 	   fileName: The name of the file containing relation's data
 */
void addRelation(struct Joiner *joiner,char *fileName);


/**
 * @brief      Calls destroyRelation for each relation 
 *             Free-s joiner
 */
void destroyJoiner(struct Joiner *joiner);


#endif