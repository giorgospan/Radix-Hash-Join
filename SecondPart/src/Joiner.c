#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Joiner.h"
#include "Relation.h"
#include "Utils.h"

void createJoiner(struct Joiner** joiner)
{

	*joiner = allocate(sizeof(struct Joiner),"createJoiner");
	(*joiner)->numOfRelations = 0;
	(*joiner)->relations      = NULL;
}

void setup(struct Joiner* joiner)
{
	/* Contains all file names : "r0\nr1\nr2\n....r20\n" */
	char *buffer   = allocate(BUFFERSIZE*sizeof(char),"setup(..)-buffer");
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
	joiner->relations = allocate(joiner->numOfRelations*sizeof(struct Relation*),"setup");

	/* Add realation corresponding to the fileName scanned from allNames */
	int offset;
	while(sscanf(allNames,"%s%n",fileName,&offset)>0)
	{
		addRelation(joiner,fileName);
		allNames+=offset;
	}
	free(buffer);
}

void addRelation(struct Joiner* joiner,char *fileName)
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

}

void destroyJoiner(struct Joiner* joiner)
{
	for (unsigned i=0;i<joiner->numOfRelations;++i) 
		destroyRelation(joiner->relations[i]);

	free(joiner->relations);
	free(joiner);
}

