#ifndef H_PHASEONE
#define H_PHASEONE

#include "usefulHeaders.h"
#include "structDefinitions.h"

// L1 data cache size in bytes [might be useful for selecting the best significantsForHash]
#define CACHE_SIZE 65536 

extern const int significantsForHash;
extern const int rangeOfValues;

/* Select the appropriate column */
uint32_t *selectColumn(uint32_t **array, uint32_t columnNumber);

/* Print a single column */
void columnPrint(uint32_t *column, uint32_t columnSize);

/* Prints An array as a row*/
void printArray(uint32_t *array, uint32_t size);

/* Print pSum */
void printPsum(uint32_t **pSum);

/* Select the appropriate column */
uint32_t *selectColumn(uint32_t **array, uint32_t columnNumber);

/* Makes structs from column */
struct PlaceHolder* convertToStructs(uint32_t *column, uint32_t columnSize);

/* Creates the histogram which has the times a hashValue occcurs in the original array 
	Histogram is of size rangeOfValues */
uint32_t* createHistogram(struct PlaceHolder* data, uint32_t columnSize);

/* Create Psum histogram which has at which offset should every hashValue start in sorted array
	pSum is also of size rangeOfValues */
uint32_t** createPsum(uint32_t* hist);

/* Delete pSum */
void deletepSum(uint32_t **pSum);

/* Finish the phase by creating the FinalArray [i.e: sorted] */
struct PlaceHolder* sortArray(struct PlaceHolder* original, uint32_t columnSize, uint32_t** pSum);

#endif