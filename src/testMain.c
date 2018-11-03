#include "usefulHeaders.h"
#include "bitWiseUtil.h"
#include "prepareInput.h"
#include "structDefinitions.h"
#include "phaseOne.h"
#include "phaseTwo.h"
#include "list.h"


int main(int argc, char const *argv[])
{
	/* Used for testing list implementation */
	///////////////////////////////////////////////////////////////
	// struct List* list;
	// struct resultTuple* t = malloc(sizeof(struct resultTuple));
	// ListCreate(&list);
	//
	// int i;
	// for(i=0;i<80000;++i)
	// {
	//
	// 	 Create tuple 
	// 	t->rowId1=i;
	// 	t->rowId2=2*i;
	//
	// 	/* Insert it to list */
	// 	ListInsert(list,t);
	// }
	//
	// ListPrint(list);
	// ListDestroy(list);
	// free(t);
	// return 0;
	////////////////////////////////////////////////////////////////


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

	/* Friendly Reminder: A relation in real life has many number of rows, 
		but we[as programmers] decide to store each row in a column 
		just to take advantage of the way arrays are stored in main memory */

	/* Find the smaller one */
	struct relation *smaller;
	if(R->cols < S->cols)
		smaller = R;
	else
		smaller = S;

	initializeIndexArray(smaller);
	buildIndexPerBucket(smaller);

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