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


void createRelation(struct Relation **rel,char *fileName)
{
	if( (*rel = malloc(sizeof(struct Relation))) == NULL )
	{
		perror("malloc failed[createRelation]");
		exit(EXIT_FAILURE);
	}
	(*rel)->columns = NULL;
	loadRelation(*rel,fileName);
}

void loadRelation(struct Relation *rel,char *fileName)
{
	int fd;

	/* Construct the correct path to file */
	char *filepath = malloc(sizeof(char)*(strlen(FILEPATH)+strlen(fileName)+1));
	strcpy(filepath,FILEPATH);
	strcat(filepath,fileName);

	/* Open relation file */
	if( (fd = open(filepath, O_RDONLY)) == -1){
		perror("open failed[loadRelation]");
		exit(EXIT_FAILURE);
	}
	free(filepath);	

	/* Find its size (in bytes) */
	struct stat sb;
	if (fstat(fd,&sb)==-1){
		perror("fstat failed[loadRelation]");
		exit(EXIT_FAILURE);
	}

	uint64_t fileSize = sb.st_size;
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
	addr+=sizeof(rel->numOfTuples);
	rel->numOfCols = *((uint64_t*) addr);
	addr+=sizeof(rel->numOfCols);

	if( (rel->columns = malloc(rel->numOfCols*sizeof(uint64_t*))) == NULL )
	{
		perror("malloc failed[loadRelation]");
		exit(EXIT_FAILURE);
	}

	/* Map every relation's column to rel->columns array */
	for (unsigned i=0;i<rel->numOfCols;++i) 
	{
		rel->columns[i] = (uint64_t*) addr;
		addr+=rel->numOfTuples*sizeof(uint64_t);
	}

	/* Closing the file does not affect mmap according to man page */
	close(fd);
}

void destroyRelation(struct Relation *rel)
{

	/**
	 * It is recommened to call munmap(...) but the process is going to terminate anyway.
	 */
	free(rel->columns);
	free(rel);
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
