#ifndef H_UTIL
#define H_UTIL 

#include "usefulHeaders.h"

/* Gets a number and return its hash Value
 depending on how many significants bits of the number are used
 IMPORTANT !!!:
  this function is hardcoded for 32 bit unsigned ints*/
uint32_t firstHash(uint32_t num, uint32_t significants);

/* Print a number in binary from Most significant to least significant
   Recursive function for ease */
void printNumInBinary(uint32_t num);


/* Retuns the length of num in bits */
/* NOT USED YET */
uint32_t sizeOfNumInBinary(uint32_t num);


#endif