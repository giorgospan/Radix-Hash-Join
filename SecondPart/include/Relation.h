#ifndef RELATION_H
#define RELATION_H

#define FILEPATH "./workloads/small/"

#include <stdint.h>


struct Relation
{
	uint64_t numOfTuples;
	uint64_t numOfCols;
	uint64_t **columns;
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
 * @brief    	Free-s any allocated space  
 */
void destroyRelation(struct Relation *rel);

#endif