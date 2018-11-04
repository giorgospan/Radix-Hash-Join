#ifndef H_PHASETHREE
#define H_PHASETHREE

#include "list.h"


/* Finds the results and inserts them into the given list */
void findResult(struct relation* big,struct relation* small,struct List* list);


/* Checks for equality between searchValue and the value stored
	in the small relation's final array.
	In addition, it calculates the rowId relevant to that array. */
void checkEqual(struct relation* small,uint32_t i,uint32_t start,uint32_t searchValue,uint32_t pseudoRow,struct List* list);

#endif