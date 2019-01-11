#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include "Intermediate.h"/*for RadixHashJoinInfo type*/

struct histArg{
  unsigned start;
  unsigned end;
  unsigned* histogram;
};

struct partitionArg{
  unsigned start;
  unsigned end;
  RadixHashJoinInfo *info;
};


void histFunc(void*);
void partitionFunc(void*);
void partition(RadixHashJoinInfo*);
void sortColumn(RadixHashJoinInfo *i);

#endif
