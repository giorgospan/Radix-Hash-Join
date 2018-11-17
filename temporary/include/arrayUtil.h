#ifndef ARRAYUTIL_H
#define ARRAYUTIL_H 

#include "usefulHeaders.h"


/* Prints 2d array of uint64_t */
void prettyArrayPrint(uint64_t**, uint64_t, uint64_t);

/* Allocate a 2d array of uint64_t */
uint64_t** allocateArray(uint64_t, uint64_t);

/* DeAllocate a 2d array */
void deAllocateArray(uint64_t **array, uint64_t);

/* DeAllocate a 2d array of Chars*/
void deAllocateArrayChar(char **array, uint64_t size);


#endif