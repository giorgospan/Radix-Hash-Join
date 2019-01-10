#ifndef PARTITION_H
#define PARTITION_H

#include <stdint.h>
#include "Intermediate.h"/*for RadixHashJoinInfo type*/

void partition(RadixHashJoinInfo*);
void sortColumn(RadixHashJoinInfo *i);

#endif
