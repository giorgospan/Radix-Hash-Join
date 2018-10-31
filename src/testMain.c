#include "usefulHeaders.h"
#include "bitWiseUtil.h"
#include "prepareInput.h"
#include "phaseOne.h"


int main(int argc, char const *argv[])
{
	srand(time(NULL));
	uint32_t value = 73;
	uint32_t size;
	printNumInBinary(value);
	printf("\nLength of num is %d\n", sizeOfNumInBinary(value));
	firstHash(value, 3);


	printf("------------------------------------------------------------------------------\n");

	
	uint32_t rows;
	uint32_t cols;
	createAndRead();
	uint32_t **p = createArrayAndInit(&rows, &cols);
	preetyArrayPrint(p, rows, cols);

	// PHASE ONE
	printf("------------------------------------------------------------------------------\n");
	printf("******************************    PHASE ONE    *******************************\n");

	uint32_t *currentColumn = selectColumn(p, 0);
	columnPrint(currentColumn, cols);

	int i;
	for (i = 0; i < cols; i++)
		printf("%u\n", currentColumn[i]);

	uint32_t sizeOfHistogram;
	struct PlaceHolder *demi = convertToStructs(selectColumn(p, 0), cols);
	// countsort(demi, cols);
	struct tuple* histoGram = findSizeOfHistogram(demi, cols, &sizeOfHistogram);
	struct tuple* pSum = createPsum(histoGram, sizeOfHistogram);
	
	struct PlaceHolder *finalR =  createSecondR(demi, cols, pSum, sizeOfHistogram);

	// printf("******************************    PHASE TWO    *******************************\n");



	/* FREE*/

	/* Either OR
		Propably the first to avoid more overhead */
	free(demi);
	// deAllocateStructs(demi);
	free(histoGram);
	free(pSum);
	deAllocateArray(p, rows);
	return 0;
}