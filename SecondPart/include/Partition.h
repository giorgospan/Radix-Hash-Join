#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include "Intermediate.h"/*for RadixHashJoinInfo type*/

#define  RADIX_BITS 8
#define  HASH_RANGE_1 256
#define  HASH_FUN_1(KEY) ((KEY) & ((1<<RADIX_BITS)-1))

void partition(RadixHashJoinInfo*);
void sortColumn(RadixHashJoinInfo *i);
unsigned *copyPsum(unsigned *original);

#endif
