#ifndef STAT_H
#define STAT_H

#include "stdint.h"

#include "Parser.h"
#include "Joiner.h"

#define PRIMELIMIT 49999991


struct columnStats
{
	uint64_t minValue;
	uint64_t maxValue;
	uint64_t f;
	uint64_t discreteValues;
	char *booleanArray;
	uint64_t sizeOfBooleanArray;
	char typeOfBooleanArray;
};

struct columnStats** allocateStatArray(uint64_t size);

void deAllocateStatArray(struct columnStats** array, uint64_t size);

/* Print a single column */
void columnPrint(uint64_t *column, uint64_t columnSize);

void findStats(uint64_t *column, struct columnStats *stat, uint64_t columnSize);

void printBooleanArray(char *array, uint64_t size);

void printColumnStats(struct columnStats *s);


void applyFilterEstimations(struct QueryInfo *q, struct Joiner *j);

void applyPredicates(struct QueryInfo *q, struct Joiner *jo);

uint64_t power(uint64_t base, uint64_t exponent);

uint64_t linearPower(uint64_t base, uint64_t exponent);

#endif
