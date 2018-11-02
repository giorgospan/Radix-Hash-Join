#include "usefulHeaders.h"
#include "bitWiseUtil.h"
#include "prepareInput.h"
#include "phaseOne.h" 


int main(int argc, char const *argv[])
{
	srand(time(NULL));
	// uint32_t value = 73;
	// uint32_t size;
	// printNumInBinary(value);
	// printf("\nLength of num is %d\n", sizeOfNumInBinary(value));
	// firstHash(value, 3);

	printf("------------------------------------------------------------------------------\n");

	createAndRead();
	uint32_t rows;
	uint32_t cols;
	uint32_t **p = createArrayAndInit(&rows, &cols);
	prettyArrayPrint(p, rows, cols);

	// PHASE ONE [Partitioning]
	// printf("------------------------------------------------------------------------------\n");
	// printf("******************************    PHASE ONE    *******************************\n");

	// uint32_t *currentColumn = selectColumn(p, 0);
	// columnPrint(currentColumn, cols);
	// uint32_t i;
	// for (i = 0; i < cols; i++)
	// 	printf("%u\n", currentColumn[i]);

	// uint32_t sizeOfHistogram;
	// struct PlaceHolder *demi = convertToStructs(selectColumn(p, 0), cols);
	// uint32_t* histoGram = createHistogram(demi, cols);
	// uint32_t** pSum = createPsum(histoGram);
	// struct PlaceHolder *finalR =  createSecondR(demi, cols, pSum);

	// PHASE TWO [Indexing]
	// printf("------------------------------------------------------------------------------\n");
	// printf("******************************    PHASE TWO    *******************************\n");




	// PHASE THREE [Finding the results]
	// printf("------------------------------------------------------------------------------\n");
	// printf("******************************    PHASE THREE    *****************************\n");



	/* FREE*/

	/* Either OR
		Propably the first to avoid more overhead */
	// deAllocateStructs(demi);
	// free(demi);
	// free(histoGram);
	// deletepSum(pSum);
	// free(finalR);
	deAllocateArray(p, rows);
	return 0;
}