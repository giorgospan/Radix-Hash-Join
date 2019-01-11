#ifndef JOINER_H
#define JOINER_H

#include <stdint.h>
#include "Parser.h"

#define  HASH_FUN_1(KEY) ((KEY) & ((1<<RADIX_BITS)-1))
extern unsigned RADIX_BITS;
extern unsigned HASH_RANGE_1;


struct Joiner
{
	struct Relation **relations;
	unsigned numOfRelations;
};


/**
 * @brief      Creates a new joiner.Also, it initializes
 *             joiner's members (relations,numOfRelations,capacity)
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
 * @brief      Executes the given query and writes
 *             its result's checksum in stdout.
 */
void join(struct Joiner *joiner,struct QueryInfo *q);

unsigned getRelationTuples(struct Joiner *joiner,unsigned relId);

void setRadixBits(struct Joiner* joiner);
void setVectorInitSize(struct Joiner *joiner);

uint64_t *getColumn(struct Joiner *joiner,unsigned relId,unsigned colId);

/**
* @brief      Calls destroyRelation for each relation
*             Free-s joiner
*/
void destroyJoiner(struct Joiner *joiner);

#endif
