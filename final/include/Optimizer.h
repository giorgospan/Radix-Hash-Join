#ifndef STAT_H
#define STAT_H

#include <limits.h>		/* for CHAR_BIT */
#include "stdint.h"

#include "Parser.h"
#include "Joiner.h"

#define PRIMELIMIT 49999991
// #define PRIMELIMIT 1500


#define BITMASK(b) (1 << ((b) % CHAR_BIT))
#define BITSLOT(b) ((b) / CHAR_BIT)
#define BITSET(v,b) ((v)[BITSLOT(b)] |= BITMASK(b))
#define BITCLEAR(v,b) ((v)[BITSLOT(b)] &= ~BITMASK(b))
#define BITTEST(v,b) ((v)[BITSLOT(b)] & BITMASK(b))
#define BITNSLOTS(nb) ((nb + CHAR_BIT - 1) / CHAR_BIT)

struct columnStats
{
	uint64_t minValue;
	uint64_t maxValue;
	unsigned f;
	unsigned discreteValues;
	char *bitVector;
	unsigned bitVectorSize;
	char typeOfBitVector;
};

void findStats(uint64_t *column, struct columnStats *stat, unsigned columnSize);
void applyColEqualityEstimations(struct QueryInfo *q, struct Joiner *j);
void filterEstimation(struct Joiner *j,struct QueryInfo *q,unsigned colId,struct columnStats *stat,unsigned actualRelId,unsigned relId,Comparison cmp,uint64_t constant);
void applyFilterEstimations(struct QueryInfo *q, struct Joiner *j);
void applyJoinEstimations(struct QueryInfo *q, struct Joiner *j);
void findOptimalJoinOrder(struct QueryInfo *q, struct Joiner *j);


/* Printing functions */
void columnPrint(uint64_t *column, unsigned columnSize);
void printBooleanArray(char *array, unsigned size);
void printColumnStats(struct columnStats *s);

#endif
