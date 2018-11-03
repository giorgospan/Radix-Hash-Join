#include "usefulHeaders.h"
#include "bitWiseUtil.h"
#include "prepareInput.h"
#include "structDefinitions.h"
#include "phaseOne.h"
#include "phaseTwo.h"


int main(int argc, char const *argv[])
{
	srand(time(NULL));
	// uint32_t value = 73;
	// uint32_t size;
	// printNumInBinary(value);
	// printf("\nLength of num is %d\n", sizeOfNumInBinary(value));
	// firstHash(value, 3);
	// printf("------------------------------------------------------------------------------\n");

	uint32_t i;	
	struct relation *R,*S;
	initializeRelation(&R);
	initializeRelation(&S);


	// PHASE ONE [Partitioning]
	printf("******************************************************************************\n");
	printf("*                              PHASE ONE                                     *\n");
	printf("******************************************************************************\n");

	////////////////////////////////////////////////
	// uint32_t *currentColumn = selectColumn(p, 0);
	// columnPrint(currentColumn, cols);			
	// for (i = 0; i < cols; i++)					
	// 	printf("%u\n", currentColumn[i]);			
	////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////
	// uint32_t sizeOfHistogram;											 
	// struct PlaceHolder *demi = convertToStructs(selectColumn(p, 0), cols);
	// uint32_t* histoGram = createHistogram(demi, cols);					 
	// uint32_t** pSum = createPsum(histoGram);								 
	// struct PlaceHolder *final =  sortArray(demi, cols, pSum);			 
	/////////////////////////////////////////////////////////////////////////

	R->demi = convertToStructs(selectColumn(R->inputRelation, R->joinCol), R->cols);
	R->histoGram = createHistogram(R->demi,R->cols);
	R->pSum = createPsum(R->histoGram);
	R->final = sortArray(R->demi,R->cols,R->pSum);

	S->demi = convertToStructs(selectColumn(S->inputRelation, S->joinCol), S->cols);
	S->histoGram = createHistogram(S->demi,S->cols);
	S->pSum = createPsum(S->histoGram);
	S->final = sortArray(S->demi,S->cols,S->pSum);

	// PHASE TWO [Indexing]
	printf("******************************************************************************\n");
	printf("*                              PHASE TWO                                     *\n");
	printf("******************************************************************************\n");

	/* Create index for every bucket of the smaller relation*/
	/* Reminder: A relation in real life has many number of rows, 
		but we[as programmers] decide to store each row in a column 
		just to take advantage of the way arrays are stored in main memory */

	// printf("R rows:%d\n",R->cols);
	// printf("S rows:%d\n",S->cols);
	if(R->cols < S->cols)
		createIndexPerBucket(R);
	else
		createIndexPerBucket(S);


	// PHASE THREE [Finding the results]
	// printf("******************************************************************************\n");
	// printf("*                              PHASE THREE                                   *\n");
	// printf("******************************************************************************\n");


	/* FREE*/

	/* Either OR
		Propably the first to avoid more overhead */

	///////////////////////////
	// deAllocateStructs(demi);
	// free(demi);			   
	// free(histoGram);		   
	// deletepSum(pSum);	   
	// free(final);			   
	///////////////////////////

	deleteRelation(R);
	deleteRelation(S);
	return 0;
}