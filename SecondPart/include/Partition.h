#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include "Intermediate.h"/*for RadixHashJoinInfo type*/

struct histArg{
  unsigned start;
  unsigned end;
  uint64_t *values;
  unsigned *histogram;
};

struct partitionArg{
  unsigned start;
  unsigned end;
  unsigned *pSumCopy;
  RadixHashJoinInfo *info;
};

void histFunc(void*);
void partitionFunc(void*);
void partition(RadixHashJoinInfo*);

#endif
