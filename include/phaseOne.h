#ifndef H_PHASEONE
#define H_PHASEONE

#include "usefulHeaders.h"
#include "structDefinitions.h"

static const int significantsForHash = 5;
static const int rangeOfValues = 32;

/* Select the appropriate column */
uint32_t *selectColumn(uint32_t **array, uint32_t columnNumber);

/* Print a single column */
void columnPrint(uint32_t *column, uint32_t columnSize);

/* Prints An array as a row*/
void printArray(uint32_t *array, uint32_t size);

/* Make structs from column */
struct PlaceHolder* convertToStructs(uint32_t *column, uint32_t columnSize);

/* Find how many hashValues are in the structs in order to declare the array Hist
	The total number of structs is the same as a column
	and Create the new array Hist */
struct tuple* findSizeOfHistogram(struct PlaceHolder* data, uint32_t columnSize, uint32_t *histSize);

/* Create Psum hist */
struct tuple* createPsum(struct tuple* hist, uint32_t histSize);

/* Finish The Phase by creating the FinalArray */
struct PlaceHolder* createSecondR(
	struct PlaceHolder* originalR, uint32_t columnSize, struct tuple* pSum, uint32_t pSumSize);

#endif