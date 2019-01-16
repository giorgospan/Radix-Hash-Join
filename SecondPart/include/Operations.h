#ifndef OPERATIONS_H
#define OPERATIONS_H

#include "Joiner.h"
#include "Vector.h"
#include "Intermediate.h"
#include "Utils.h"


struct filterArg{
  uint64_t *col;
  uint64_t constant;
  Comparison cmp ;
  unsigned start;
  unsigned end;
  struct Vector **vector;
};
void filterFunc(void *arg);



void colEqualityInter(uint64_t *leftCol,uint64_t *rightCol,unsigned posLeft,unsigned posRight,struct Vector **vector);
void colEquality(uint64_t *leftCol,uint64_t *rightCol,unsigned numOfTuples,struct Vector **vector);
void filter(uint64_t *col,Comparison cmp,uint64_t constant,unsigned numOfTuples,struct Vector **vector);
void filterInter(uint64_t *col,Comparison cmp,uint64_t constant,struct Vector **vector);
void joinNonInterNonInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right);
void joinNonInterInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right);
void joinInterNonInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right);
void joinInterInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right);
#endif
