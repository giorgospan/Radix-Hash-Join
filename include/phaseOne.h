#ifndef H_PHASEONE
#define H_PHASEONE

#include "usefulHeaders.h"
#include "structDefinitions.h"

// L1 data cache size in bytes [might be useful for selecting the best significantsForHash]
#define CACHE_SIZE 65536 

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

/* Ceates the histogram which has the times a hashValue occcurs in the originalR 
	Histogram is of size rangeOfValues */
uint32_t* createHistogram(struct PlaceHolder* data, uint32_t columnSize);

/* Create Psum histogram which has at which offset should every hashValue start in secondR
	pSum is also of size rangeOfValues */
uint32_t** createPsum(uint32_t* hist);

/* Print pSum */
void printPsum(uint32_t **pSum);

/* Delete pSum */
void deletepSum(uint32_t **pSum);

/* Finish The Phase by creating the FinalArray */
struct PlaceHolder* createSecondR(struct PlaceHolder* originalR, uint32_t columnSize, uint32_t** pSum);

#endif