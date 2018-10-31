#ifndef H_PREPARE
#define H_PREPARE

#include "usefulHeaders.h"


/* Creates the binary Input file*/
void inputCreator();

/* Reads the binaty input file since it; not readable like a .txt*/
void inputReader();

/* Just test if everything is ok
   OR CALL IT TO GET STARTED WITH CREATING THE INPUT */
void createAndRead();

/* Prints 2d array of uint32_t */
void preetyArrayPrint(uint32_t**, uint32_t, uint32_t);

/* Allocate a 2d array of uint32_t */
uint32_t** allocateArray(uint32_t, uint32_t);

/* DeAllocate a 2d array */
void deAllocateArray(uint32_t **array, uint32_t);

/* Create array and fill it with the input
	and store the values in "pseudo" column major indexing
	We pass in two pointers to return the sizes of the array's dimensions */
uint32_t** createArrayAndInit(uint32_t *rowSize, uint32_t *colSize);


#endif
