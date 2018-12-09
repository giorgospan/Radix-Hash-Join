#ifndef BUILD_H
#define BUILD_H

#include "Partition.h"/* for "RadixHashJoinInfo" type*/

#define  HASH_RANGE_2 101
#define  HASH_FUN_2(KEY) ((KEY)%(101))

void build(RadixHashJoinInfo *infoLeft,RadixHashJoinInfo *infoRight);
void initializeIndexArray(RadixHashJoinInfo *info);
void buildIndexPerBucket(RadixHashJoinInfo *info);
void traverseChain(unsigned chainPos,unsigned* chainArray,unsigned posToBeStored);

#endif