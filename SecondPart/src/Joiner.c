#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Joiner.h"
#include "Relation.h"
#include "Intermediate.h"
#include "Vector.h"
#include "Utils.h"

unsigned RADIX_BITS;
unsigned HASH_RANGE_1;

void createJoiner(struct Joiner **joiner)
{
	*joiner = malloc(sizeof(struct Joiner));
	MALLOC_CHECK(*joiner);
	(*joiner)->numOfRelations = 0;
	(*joiner)->relations      = NULL;
}

void setup(struct Joiner *joiner)
{
	/* Contains all file names : "r0\nr1\nr2\n....r20\n" */
	char *buffer   = malloc(BUFFERSIZE*sizeof(char));
	MALLOC_CHECK(buffer);
	char *allNames = buffer;

	/* We assume that file name will be at most 18[do not forget '\n' and '\0'] characters long */
	char fileName[20];

	/* Get number of relations and store file names to allNames */
	allNames[0] = '\0';
	while (fgets(fileName, sizeof(fileName), stdin) != NULL)
	{
		if(!strcmp(fileName,"Done\n"))
			break;
		++joiner->numOfRelations;
		strcat(allNames,fileName);
	}

	/* Allocate space to store relations */
	joiner->relations = malloc(joiner->numOfRelations*sizeof(struct Relation*));
	MALLOC_CHECK(joiner->relations);

	/* Add realation corresponding to the fileName scanned from allNames */
	int offset;
	while(sscanf(allNames,"%s%n",fileName,&offset)>0)
	{
		addRelation(joiner,fileName);
		allNames+=offset;
	}
	setRadixBits(joiner);
 	setVectorInitSize(joiner);
	free(buffer);
}

void addRelation(struct Joiner *joiner,char *fileName)
{

	/* Indicates the number of relations added so far */
	static unsigned i=0;

	/* Create a new relation */
	struct Relation *rel;
	createRelation(&rel,fileName);

	/* Add it to joiner's "relations" array */
	joiner->relations[i++] = rel;
	// dumpRelation(rel,fileName);
}

void join(struct Joiner *joiner,struct QueryInfo *q)
{
	struct InterMetaData *inter;
	createInterMetaData(&inter,q);

	// fprintf(stderr,"=========================================================\n");
	// fprintf(stderr,"Column Equalities\n");
	// fprintf(stderr,"=========================================================\n");
	applyColumnEqualities(inter,joiner,q);

	// fprintf(stderr,"=========================================================\n");
	// fprintf(stderr,"Filters\n");
	// fprintf(stderr,"=========================================================\n");
	applyFilters(inter,joiner,q);

	// fprintf(stderr,"=========================================================\n");
	// fprintf(stderr,"Joins\n");
	// fprintf(stderr,"=========================================================\n");
	applyJoins(inter,joiner,q);


	// fprintf(stderr,"=========================================================\n");
	// fprintf(stderr,"CheckSums\n");
	// fprintf(stderr,"=========================================================\n");
	applyCheckSums(inter,joiner,q);


	// fprintf(stderr,"=========================================================\n");
	// fprintf(stderr,"Destruction\n");
	// fprintf(stderr,"=========================================================\n");
	destroyInterMetaData(inter);
}

uint64_t *getColumn(struct Joiner *joiner,unsigned relId,unsigned colId)
{
	return joiner->relations[relId]->columns[colId];
}

unsigned getRelationTuples(struct Joiner *joiner,unsigned relId)
{
	return joiner->relations[relId]->numOfTuples;
}

void setVectorInitSize(struct Joiner *joiner)
{
	/**
	 * small	: 1000
	 * medium	: 1000
	 * large	: 5000
	 * public	: 500000
	 */

	unsigned sum            = 0;
	unsigned avgNumOfTuples = 0;
	for(unsigned i=0;i<joiner->numOfRelations;++i)
		sum+=joiner->relations[i]->numOfTuples;
	avgNumOfTuples = sum/joiner->numOfRelations;

	if(avgNumOfTuples<500000)
		initSize = 1000;
	else if(avgNumOfTuples<1200000)
		initSize = 1000;
	else if(avgNumOfTuples<2000000)
		initSize = 5000;
	else
		initSize = 500000;
}

void setRadixBits(struct Joiner *joiner)
{
	unsigned sum            = 0;
	unsigned avgNumOfTuples = 0;
	for(unsigned i=0;i<joiner->numOfRelations;++i)
		sum+=joiner->relations[i]->numOfTuples;
	avgNumOfTuples = sum/joiner->numOfRelations;

	/**
	 * small	: 4,16
	 * medium	: 5,32
	 * large	: 5,32
	 * public	: 8,256
	 */

	 if (avgNumOfTuples<500000) {
		 	RADIX_BITS   = 4;
			HASH_RANGE_1 = 16;
	} else if (avgNumOfTuples<2000000) {
			RADIX_BITS   = 5;
			HASH_RANGE_1 = 32;
	 } else {
			RADIX_BITS   = 8;
			HASH_RANGE_1 = 256;
	 }
}

void destroyJoiner(struct Joiner *joiner)
{
	for (unsigned i=0;i<joiner->numOfRelations;++i)
		destroyRelation(joiner->relations[i]);
	free(joiner->relations);
	free(joiner);
}
