#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Joiner.h"
#include "Relation.h"


void createJoiner(struct Joiner** joiner)
{
	if( (*joiner = malloc(sizeof(struct Joiner))) == NULL )
	{
		perror("malloc failed[createJoiner]");
		exit(EXIT_FAILURE);
	}
	(*joiner)->capacity       = 15;
	(*joiner)->numOfRelations = 0;
	(*joiner)->relations      = NULL;
}

void setup(struct Joiner* joiner)
{
	/**
	 * At first, we allocate enough space for 15 relations.
	 * In case of more relations, we'll use realloc().
	 */
	if( (joiner->relations = malloc(15*sizeof(struct Relation*))) == NULL )
	{
		perror("malloc failed[setup]");
		exit(EXIT_FAILURE);
	}

	/* We assume that file name will be at most 20 characters long */
	char fileName[20];
	while (fgets(fileName, sizeof(fileName), stdin) != NULL)
	{
		if(!strcmp(fileName,"Done\n"))
			break;
		fileName[strcspn(fileName, "\n")] = 0;
		addRelation(joiner,fileName);
	}
}

void addRelation(struct Joiner* joiner,char *fileName)
{	
	uint64_t currentSize = joiner->numOfRelations;
	struct Relation **currentPtr = joiner->relations;

	/* Allocate space for twice more relations */
	if(currentSize == joiner->capacity)
	{
		uint64_t newSize = currentSize*2;
		if(  (joiner->relations = realloc(currentPtr,(newSize)*sizeof(struct Relation*))) == NULL )
		{
			perror("realloc failed[addRelation]");
			exit(EXIT_FAILURE);
		}
		joiner->capacity = newSize;		
	}

	/* Create a new relation */
	struct Relation *rel;
	createRelation(&rel,fileName); 

	/* Add it to joiner's array with relations */
	joiner->relations[joiner->numOfRelations++] = rel;
	printf("Successful creation of a new relation using file \"%s\"\n",fileName);
}


void destroyJoiner(struct Joiner* joiner)
{
	for (unsigned i=0;i<joiner->numOfRelations;++i) 
		destroyRelation(joiner->relations[i]);

	free(joiner->relations);
	free(joiner);
}
