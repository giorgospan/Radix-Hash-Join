#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

#include "Relation.h"
#include "Utils.h"
#include "Optimizer.h"


void createRelation(struct Relation **rel,char *fileName)
{
	*rel = malloc(sizeof(struct Relation));
	MALLOC_CHECK(*rel);
	(*rel)->columns = NULL;
	loadRelation(*rel,fileName);

	// Allocate space for stats
	(*rel)->stats = malloc((*rel)->numOfCols*sizeof(struct columnStats));
	MALLOC_CHECK((*rel)->stats);

	// Calculate stats for every column of the relation
	for (unsigned i = 0; i < (*rel)->numOfCols; ++i)
	{
		findStats((*rel)->columns[i], &(*rel)->stats[i], (*rel)->numOfTuples);
		// fprintf(stderr, "Relation[%s]\n",fileName);
		// printColumnStats(&(*rel)->stats[i]);
	}
	// fprintf(stderr, "\n\n\n");
}

void loadRelation(struct Relation *rel,char *fileName)
{
	int fd;

	/* Open relation file */
	if( (fd = open(fileName, O_RDONLY)) == -1){
		perror("open failed[loadRelation]");
		exit(EXIT_FAILURE);
	}

	/* Find its size (in bytes) */
	struct stat sb;
	if (fstat(fd,&sb)==-1){
		perror("fstat failed[loadRelation]");
		exit(EXIT_FAILURE);
	}

	unsigned fileSize = sb.st_size;
	if (fileSize<16){
		fprintf(stderr,"Relation file \"%s\" does not contain a valid header",fileName);
		exit(EXIT_FAILURE);
	}

	/* Map file to memory */
	char *addr;
	if(  (addr = mmap(NULL,fileSize,PROT_READ,MAP_PRIVATE,fd,0u)) == MAP_FAILED ){

		perror("mmap failed[loadRelation]");
		exit(EXIT_FAILURE);
	}

	/* Fetch numOfTuples & numOfCols */
	rel->numOfTuples = *((uint64_t*) addr);
	addr+=sizeof((uint64_t)rel->numOfTuples);
	rel->numOfCols = *((uint64_t*) addr);
	addr+=sizeof((uint64_t)rel->numOfCols);

	rel->columns = malloc(rel->numOfCols*sizeof(uint64_t*));
	MALLOC_CHECK(rel->columns);

	/* Map every relation's column to rel->columns array */
	for (unsigned i=0;i<rel->numOfCols;++i)
	{
		rel->columns[i] = (uint64_t*) addr;
		addr+=rel->numOfTuples*sizeof(uint64_t);
	}

	/* Closing the file does not affect mmap according to man page */
	close(fd);
}

void dumpRelation(struct Relation *rel,char *fileName)
{
	/* Create path */
	char path[100] = "../../dumpFiles/";
	strcat(path,fileName);
	strcat(path,".dump");

	FILE* fp;
	if(  (fp=fopen(path,"w"))==NULL)
	{
		perror("fopen failed[dumpRelation]");
		exit(EXIT_FAILURE);
	}

	for (unsigned i=0;i<rel->numOfTuples;++i)
	{
		for(unsigned j=0;j<rel->numOfCols;++j)
			fprintf(fp,"%lu|", rel->columns[j][i]);
		fprintf(fp,"\n");
	}
	fclose(fp);
}

void printRelation(struct Relation *rel)
{
	for (unsigned i=0;i<rel->numOfTuples;++i)
	{
		for(unsigned j=0;j<rel->numOfCols;++j)
			printf("%lu|", rel->columns[j][i]);
		printf("\n");
	}
}

void destroyRelation(struct Relation *rel)
{
	/**
	 * It is recommened to call munmap(...) but the process is going
	 * to terminate anyway afterwards.
	 */
	for(unsigned i=0;i<rel->numOfCols;++i)
		free(rel->stats[i].bitVector);
	free(rel->stats);
	free(rel->columns);
	free(rel);
}
