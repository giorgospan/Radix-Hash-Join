#ifndef SETUP_H
#define SETUP_H

#include "usefulHeaders.h"
#include "structDefinitions.h"

/* Prepare the basis */
void setup();

/* Read the relation file names , return them as an array of char** 
	and return the total number of relations to be read*/
char** readRelationFileNames(uint64_t *numOfRelations);

/* Use mmap to get them to main memory */
uint64_t*** mapRelationsToMemory(char **relationNames, uint64_t numOfRelations, struct placeHolder** interfaceArray);

/* Create array and fill it with the input
	and store the values in "pseudo" column major indexing
	We pass in two pointers to return the sizes of the array's dimensions */
uint64_t** createArrayAndInit(uint64_t *rowSize, uint64_t *colSize, FILE *f);


#endif