#include "usefulHeaders.h"
#include "bitWiseUtil.h"
#include "prepareInput.h"
#include "structDefinitions.h"
#include "phaseOne.h"
#include "phaseTwo.h"
#include "phaseThree.h"
#include "list.h"


int main(int argc, char const *argv[])
{
	///////////////////////////////////////////////////////////////////////////////////////////////
	// srand(time(NULL));
	// uint32_t value = 73;
	// uint32_t size;
	// printNumInBinary(value);
	// printf("\nLength of num is %d\n", sizeOfNumInBinary(value));
	// firstHash(value, 3);
	// printf("------------------------------------------------------------------------------\n");
	///////////////////////////////////////////////////////////////////////////////////////////////

	clock_t begin,end;
	double duration;
	uint32_t i;
	struct relation *R,*S;
	initializeRelation(&R);
	initializeRelation(&S);

	// PHASE ONE [Partitioning]
	printf("******************************************************************************\n");
	printf("*                              PHASE ONE                                     *\n");
	printf("******************************************************************************\n");
	begin = clock();

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

	end = clock();
	duration = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Duration: %.3f\n\n\n",duration);

	// PHASE TWO [Indexing]
	printf("******************************************************************************\n");
	printf("*                              PHASE TWO                                     *\n");
	printf("******************************************************************************\n");
	begin = clock();

	/* Friendly Reminder: A relation in real life has many number of rows,
		but we[as programmers] decide to store each row in a column
		just to take advantage of the way arrays are stored in main memory */

	/* Build index on every bucket of the smaller relation */
	struct relation *small,*big;

	if(R->cols < S->cols)
	{
		big = S;
		small = R;
	}
	else
	{
		big = R;
		small = S;
	}

	initializeIndexArray(small);
	buildIndexPerBucket(small);

	end = clock();
	duration = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Duration: %.3f\n\n\n",duration);

	// PHASE THREE [Finding the results]
	printf("******************************************************************************\n");
	printf("*                              PHASE THREE                                   *\n");
	printf("******************************************************************************\n");
	begin = clock();

	/* A list to hold the result tuples */
	struct List *list;
	ListCreate(&list);

	/* Find results */
	findResult(big,small,list);

	end = clock();
	duration = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("Duration: %.3f\n\n\n",duration);

	/* Print results to file "results.log" */
	ListPrint(list);


	/* FREE */
	ListDestroy(list);
	deleteRelation(R);
	deleteRelation(S);
	return 0;
}
