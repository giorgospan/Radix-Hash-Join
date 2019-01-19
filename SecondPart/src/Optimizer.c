#include <stdio.h>
#include <stdlib.h>
#include "Optimizer.h"
#include "Utils.h"
#include "Parser.h"
#include "Joiner.h"
#include "Relation.h"


struct columnStats** allocateStatArray(unsigned size)
{
	struct columnStats** temp = malloc(sizeof(struct columnStats*) * size);
	MALLOC_CHECK(temp);

	for (unsigned i = 0; i < size; i++)
	{
		temp[i] = malloc(sizeof(struct columnStats));
		MALLOC_CHECK(temp[i]);
		temp[i]->bitVector = NULL;
	}
	return temp;
}

void deAllocateStatArray(struct columnStats **array, unsigned size)
{
	for (unsigned i = 0; i < size; i++)
	{
		free(array[i]->bitVector);
		free(array[i]);
	}
	free(array);
}

void findStats(uint64_t *column, struct columnStats *stat, unsigned columnSize)
{
	/* Find MIN and MAX */
	uint64_t min = column[0];
	uint64_t max = column[0];
	for (unsigned i = 1; i < columnSize; ++i)
	{
		if (column[i] > max)
			max = column[i];
		if (column[i] < min)
			min = column[i];
	}

	/* Find discrete values */
	unsigned nbits  = (max - min + 1 > PRIMELIMIT) ? PRIMELIMIT : max - min + 1;
	stat->bitVector = calloc(BITNSLOTS(nbits),CHAR_BIT);
	MALLOC_CHECK(stat->bitVector);
	stat->discreteValues = 0;


	/* Find the way to fill the boolean array depending on its size relative to PRIMELIMIT */
	if (nbits != PRIMELIMIT)
	{
		stat->typeOfBitVector = 0;
		for (unsigned i = 0; i < columnSize; ++i)
		{
			if(BITTEST(stat->bitVector,column[i]-min) == 0)
				(stat->discreteValues)++;
			BITSET(stat->bitVector,column[i]-min);
		}
	}
	else
	{
		stat->typeOfBitVector = 1;
		for (unsigned i = 0; i < columnSize; ++i)
		{
			if(BITTEST(stat->bitVector,(column[i]-min) % PRIMELIMIT) == 0)
				(stat->discreteValues)++;
			BITSET(stat->bitVector,(column[i]-min) % PRIMELIMIT);
		}
	}
	/* Assign the remaining values to the stats*/
	stat->minValue      = min;
	stat->maxValue      = max;
	stat->f             = columnSize;
	stat->bitVectorSize = nbits;
}


void applyColEqualityEstimations(struct QueryInfo *q, struct Joiner *j)
{
	for(unsigned i = 0 ; i < q->numOfPredicates; ++i)
	{
		unsigned leftRelId        = getRelId(&q->predicates[i].left);
		unsigned rightRelId       = getRelId(&q->predicates[i].right);
		unsigned leftColId        = getColId(&q->predicates[i].left);
		unsigned rightColId       = getColId(&q->predicates[i].right);
		unsigned actualId1        = getOriginalRelId(q, &q->predicates[i].left);
		unsigned actualId2        = getOriginalRelId(q, &q->predicates[i].right);
		struct columnStats *stat1 = (*(j->relations[actualId1])).colStats[leftColId];
		struct columnStats *stat2 = (*(j->relations[actualId2])).colStats[rightColId];
		struct columnStats *temp;

		// Same relation - different columns
		if(isColEquality(&q->predicates[i]) && (leftColId != rightColId))
		{
			// fprintf(stderr,"%u.%u=%u.%u & ",leftRelId,leftColId,rightRelId,rightColId);
			// Find estimations for the two columns
			uint64_t newMin;
			uint64_t newMax;
			unsigned newF;
			unsigned newD;
			newMin = (stat1->minValue > stat2->minValue) ? stat1->minValue : stat2->minValue;
			newMax = (stat1->maxValue < stat2->maxValue) ? stat1->maxValue : stat2->maxValue;
			newF   = stat1->f / (newMax - newMin + 1);
			newD   = stat1->discreteValues * (1-power(1-(newF/stat1->f),stat1->f/stat1->discreteValues));

			/* Update the statistics of every other column except for the ones taking part in the equality */
			for (unsigned c = 0; c < (*(j->relations[actualId1])).numOfCols; ++c)
			{
				temp = (*(j->relations[actualId1])).colStats[c];
				if ((c!=leftColId)  && (c!=rightColId))
				{
					/* In case stat1->f or temp->discreteValues is zero */
					stat1->f = (stat1->f == 0) ? 1 : stat1->f;
					temp->discreteValues = (temp->discreteValues == 0) ? 1 : temp->discreteValues;

					temp->discreteValues = temp->discreteValues * (1-power(1-(newF/stat1->f),temp->f/temp->discreteValues));
				}
			}

			/* Update the statistics for the two columns taking part in the equality */
			stat1->minValue       = stat2->minValue       = newMin;
			stat1->maxValue       = stat2->maxValue       = newMax;
			stat1->f              = stat2->f              = newF;
			stat1->discreteValues = stat2->discreteValues = newD;
		}
	}
}


void filterEstimation(struct Joiner *j,unsigned colId,struct columnStats *stat,unsigned actualRelId,Comparison cmp,uint64_t constant)
{
	struct columnStats *temp;
	uint64_t fTemp = stat->f;
	char isInArray = 0;

	if (cmp == '=')
	{

		/* Find if constant is in the discrete values */
		/* If constant is not in the range of values of the column we won't find it in the bitVector */
		if ( (constant < stat->minValue) ||  (constant > stat->maxValue) )
		isInArray = 0;
		/* If the value in the bitVector is 1 then the constant is in the bitVector */
		else
		{
			/* Make sure we use the correct way to find if it is in the bitVector */
			if (stat->typeOfBitVector == 0)
			{
				if (stat->bitVector[constant - stat->minValue] == 1)
				isInArray = 1;
			}
			else if (stat->typeOfBitVector == 1)
			{
				if (stat->bitVector[(constant - stat->minValue) % PRIMELIMIT] == 1)
				isInArray = 1;
			}
		}

		/* Change the statistics  of the column */
		stat->minValue = constant;
		stat->maxValue = constant;
		if (isInArray == 0)
		{
			stat->f = 0;
			stat->discreteValues = 0;
		}
		else
		{
			stat->f = stat->f / stat->discreteValues;
			stat->discreteValues = 1;
		}
	}
	else
	{

		/* cmp is '>' or '<' */
		/* lowerLimit */
		uint64_t k1 = (cmp == '>') ?  constant + 1 : stat->minValue ;
		/* upperLimit */
		uint64_t k2 = (cmp == '<') ?  constant - 1 : stat->maxValue;

		if (k1 < stat->minValue)
			k1 = stat->minValue;

		if (k2 > stat->maxValue)
			k2 = stat->maxValue;

		/* If factor is close in (0,1),round it up to 1 */
		double factor = (double)(k2 - k1) / (stat->maxValue - stat->minValue);
		if((factor < 1)  && (factor > 0))
			factor = 1;

		fprintf(stderr, "k1:%lu | k2:%lu\n\n",k1,k2);
		// fprintf(stderr, "minPrev:%lu | maxPrev:%lu\n",stat->minValue,stat->maxValue);
		// fprintf(stderr, "Factor:%.3lf\n",factor);

		/* Change the statistics  of the column */
		if(stat->maxValue - stat->minValue > 0)
		{
			stat->discreteValues = factor * stat->discreteValues;
			stat->f              = factor * stat->f;
		}
		else
			/* In case of: R.A<CONSTANT & R.A>CONSTANT */
			stat->discreteValues = stat->f  = 0 ;

		stat->minValue = k1;
		stat->maxValue = k2;

		// fprintf(stderr, "stat->f:%u\n\n\n",stat->f );
	}

	/* Update the statistics of every other column
	The formulas are the same for every filter */
	for (unsigned c = 0; c < (*(j->relations[actualRelId])).numOfCols; ++c)
	{
		temp = (*(j->relations[actualRelId])).colStats[c];
		/* Make sure we update every other column , except the one that we just applied the filter estimations  */
		if ((c != colId))
		{
			/* In case fTemp or temp->discreteValues is zero */
			fTemp = (fTemp == 0) ? 1 : fTemp;
			temp->discreteValues = (temp->discreteValues == 0) ? 1 : temp->discreteValues;

			temp->discreteValues = temp->discreteValues * (1 -
				power(1 - (1 - stat->f / fTemp),
				temp->f / temp->discreteValues));
				temp->f = stat->f;
			}
		}
}

void applyFilterEstimations(struct QueryInfo *q, struct Joiner *j)
{

	for(unsigned i = 0 ; i < q->numOfFilters; ++i)
	{
		unsigned relId       = getRelId(&q->filters[i].filterLhs);
		unsigned colId       = getColId(&q->filters[i].filterLhs);
		Comparison cmp       = getComparison(&q->filters[i]);
		uint64_t constant    = getConstant(&q->filters[i]);
		unsigned actualRelId = getOriginalRelId(q, &q->filters[i].filterLhs);;

		// fprintf(stderr, "%s\n", "::::::::::::::::::::::::::::::::::::::::::::::::::");
		// fprintf(stderr,"%u.%u%c%ld",relId,colId,cmp,constant);
		// fprintf(stderr, "\n%s\n", "::::::::::::::::::::::::::::::::::::::::::::::::::");
		struct columnStats *stat = (*(j->relations[actualRelId])).colStats[colId];
		filterEstimation(j,colId,stat,actualRelId,cmp,constant);
	}
}

void applyJoinEstimations(struct QueryInfo *q, struct Joiner *j)
{

	/* Find stats for columns in predicates */
	for(unsigned i=0;i<q->numOfPredicates;++i)
	{
		unsigned leftRelId            = getRelId(&q->predicates[i].left);
		unsigned rightRelId           = getRelId(&q->predicates[i].right);
		unsigned leftColId            = getColId(&q->predicates[i].left);
		unsigned rightColId           = getColId(&q->predicates[i].right);
		unsigned actualRelIdLeft      = getOriginalRelId(q, &q->predicates[i].left);
		unsigned actualRelIdRight     = getOriginalRelId(q, &q->predicates[i].right);
		struct columnStats *statLeft  = (*(j->relations[actualRelIdLeft])).colStats[leftColId];
		struct columnStats *statRight = (*(j->relations[actualRelIdRight])).colStats[rightColId];


		if(!isColEquality(&q->predicates[i]))
		{
			fprintf(stderr,"%u.%u=%u.%u &\n",leftRelId,leftColId,rightRelId,rightColId);
			uint64_t max,min;
			min = (statLeft->minValue > statRight->minValue) ? statLeft->minValue : statRight->minValue;
			max = (statLeft->maxValue < statRight->maxValue) ? statLeft->maxValue : statRight->maxValue;

			/* Implicit filter for min value */
			filterEstimation(j,leftColId,statLeft,actualRelIdLeft,'>',min);
			filterEstimation(j,rightColId,statRight,actualRelIdRight,'>',min);

			/* Implicit filter for max value */
			filterEstimation(j,leftColId,statLeft,actualRelIdLeft,'<',max);
			filterEstimation(j,rightColId,statRight,actualRelIdRight,'<',max);

			// printColumnStats(statLeft);
			// printColumnStats(statRight);
		}
	}
	fprintf(stderr, "\n");
}

uint64_t power(uint64_t base, uint64_t exponent)
{
	if (exponent == 0)
		return 1;
	else if (exponent % 2 == 0)
	{
		uint64_t temp = power(base, exponent / 2);
		return temp * temp;
	}
	else
		return base * power(base, exponent - 1);
}

uint64_t linearPower(uint64_t base, uint64_t exponent)
{
	uint64_t res = 1;
	for (uint64_t i = 0; i < exponent; ++i)
		res *= base;
	return res;
}

void printColumnStats(struct columnStats *s)
{
	fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	fprintf(stderr, "minValue: %lu\n", s->minValue);
	fprintf(stderr, "maxValue: %lu\n", s->maxValue);
	fprintf(stderr, "f: %u\n", s->f);
	fprintf(stderr, "discreteValues: %u\n", s->discreteValues);
	fprintf(stderr, "Type of boolean array is: %d\n", s->typeOfBitVector);
	fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
