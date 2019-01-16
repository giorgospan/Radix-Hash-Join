#ifndef RELATION_H
#define RELATION_H

#include <stdint.h>
#include "Optimizer.h"


struct Relation
{
	unsigned numOfTuples;
	unsigned numOfCols;
	uint64_t **columns;
	struct columnStats **colStats;
};

/**
 * @brief   	Creates a new relation and calls
 *           	loadRelation() to retrieve relation's data.
 */
void createRelation(struct Relation **rel,char *fileName);


/**
 * @brief      mmap-s relation's data from the given file
 */
void loadRelation(struct Relation *rel,char *fileName);

/**
 * @brief      Typical printing function
 */
void printRelation(struct Relation *rel);

/**
 * @brief      Dumps relation to the given file
 */
void dumpRelation(struct Relation *rel,char *fileName);

/**
 * @brief    	Free-s any allocated space  
 */
void destroyRelation(struct Relation *rel);

#endif