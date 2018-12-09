#ifndef PROBE_H
#define PROBE_H
#include "Vector.h"
#include "Partition.h"

struct Vector *probe(RadixHashJoinInfo *left,RadixHashJoinInfo *right,struct Vector *results);

/**
 * @brief      Checks for equality between the two column values and inserts to the
 *             results vector a tuple constructed from the two tuples [one from each column]
 *
 * @param      small        The small column [has been indexed]
 * @param      big          The big column   [non-indexed]
 * @param[in]  i            Row for the big column
 * @param[in]  start        Starting position of the small column's bucket 
 * @param[in]  searchValue  The search value
 * @param[in]  pseudoRow    The bucket row [will use it to construct the original row]
 * @param      results      The results vector
 */
void checkEqual(RadixHashJoinInfo *small,RadixHashJoinInfo *big,unsigned i,unsigned start,unsigned searchValue,unsigned pseudoRow,struct Vector *results);
void constructTuple(RadixHashJoinInfo *small,RadixHashJoinInfo *big,unsigned actualRow,unsigned i,unsigned *tuple);
RadixHashJoinInfo *getLeftCol(RadixHashJoinInfo *i1,RadixHashJoinInfo *i2);
RadixHashJoinInfo *getRightCol(RadixHashJoinInfo *i1,RadixHashJoinInfo *i2);

#endif



