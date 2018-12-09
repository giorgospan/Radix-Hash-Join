#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include "Intermediate.h"/*for JoinArg type*/

#define  RADIX_BITS 4
#define  HASH_RANGE_1 16  
#define  HASH_FUN_1(KEY) ((KEY) & ((1<<RADIX_BITS)-1))

typedef struct ColumnInfo
{
	uint64_t *values;
	unsigned *rowIds;
	struct Vector *tuples;
}ColumnInfo;

typedef struct Index
{
	/* These are the two arrays
		used for the indexing of a single bucket */
	unsigned *chainArray;
	unsigned *bucketArray;	
}Index;

/* Contains info about the column we are joining */
typedef struct RadixHashJoinInfo
{
	unsigned columnSize;
	unsigned tupleSize;
	ColumnInfo *unsorted;
	ColumnInfo *sorted;
	unsigned *hist;
	unsigned *pSum;
	Index **indexArray;
	// Will be set to 1 during build phase, in case this is the small column
	// between the two columns we're joining.
	unsigned isSmall;
	// 1: if it is on join's lhs, 0 otherwise
	unsigned isLeft;
}RadixHashJoinInfo;


RadixHashJoinInfo* partition(JoinArg*);
void sortColumn(RadixHashJoinInfo *i);
unsigned *copyPsum(unsigned *original);
void destroyRadixHashJoinInfo(RadixHashJoinInfo *);
void destroyColumnInfo(ColumnInfo **c);

#endif