#include <stdio.h>
#include <stdlib.h>
#include "Optimizer.h"
#include "Utils.h"
#include "Parser.h"
#include "Joiner.h"
#include "Relation.h"

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
		unsigned actualId         = getOriginalRelId(q, &q->predicates[i].left);
		struct columnStats *stat1 = &q->estimations[leftRelId][leftColId];
		struct columnStats *stat2 = &q->estimations[rightRelId][rightColId];
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

			// fprintf(stderr, "stat1: %lu ~~> %lu\n",stat1->minValue,stat1->maxValue);
			// fprintf(stderr, "stat2: %lu ~~> %lu\n",stat2->minValue,stat2->maxValue);

			newMin = (stat1->minValue > stat2->minValue) ? stat1->minValue : stat2->minValue;
			newMax = (stat1->maxValue < stat2->maxValue) ? stat1->maxValue : stat2->maxValue;
			newF   = stat1->f / (newMax - newMin + 1);
			newD   = stat1->discreteValues * (1-power(1-(newF/stat1->f),stat1->f/stat1->discreteValues));

			/* Update the statistics of every other column except for the ones taking part in the equality  */
			/* Note: Number of columns of every relation is stored in joiner, thus we need joiner to access it */
			for (unsigned c = 0; c < (*(j->relations[actualId])).numOfCols; ++c)
			{
				/* leftRelId and rightRelId are the same, so we could also use rightRelId */
				temp = &q->estimations[leftRelId][c];
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
			// printColumnStats(&q->estimations[leftRelId][1]);
		}
		// Same relation - same column
		else if(isColEquality(&q->predicates[i]))
		{
			stat1->f = (stat1->f * stat1->f) / (stat1->maxValue - stat1->minValue + 1);
			for (unsigned c = 0; c < (*(j->relations[actualId])).numOfCols; ++c)
			{
				/* leftRelId and rightRelId are the same, so we could also use rightRelId */
				/* nothing changes in this case,apart from "f" */
				temp = &q->estimations[leftRelId][c];
				if (c!=rightColId)
					temp->f = stat1->f;
			}
		}
	}
}

void filterEstimation(struct Joiner *j,struct QueryInfo *q,unsigned colId,struct columnStats *stat,unsigned actualRelId,unsigned relId,Comparison cmp,uint64_t constant)
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
				// fprintf(stderr, "%u\n",BITTEST(stat->bitVector,522 - stat->minValue));
				if (BITTEST(stat->bitVector,constant - stat->minValue) != 0)
				{
					isInArray = 1;
				}
			}
			else if (stat->typeOfBitVector == 1)
			{
				if(BITTEST(stat->bitVector,(constant - stat->minValue) % PRIMELIMIT) != 0)
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

		/* If factor is in (0,1],round it up to 1 */
		double factor = (double)(k2 - k1) / (stat->maxValue - stat->minValue);
		if((factor <= 1)  && (factor > 0))
			factor = 1;

		// fprintf(stderr, "k1:%lu | k2:%lu\n\n",k1,k2);
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
	}

	/* Update the statistics of every other column
	The formulas are the same for every filter */
	for (unsigned c = 0; c < (*(j->relations[actualRelId])).numOfCols; ++c)
	{
		temp = &q->estimations[relId][c];
		/* Make sure we update every other column , except the one that we just applied the filter estimations  */
		if (c != colId)
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
		unsigned relId           = getRelId(&q->filters[i].filterLhs);
		unsigned colId           = getColId(&q->filters[i].filterLhs);
		Comparison cmp           = getComparison(&q->filters[i]);
		uint64_t constant        = getConstant(&q->filters[i]);
		unsigned actualRelId     = getOriginalRelId(q, &q->filters[i].filterLhs);
		struct columnStats *stat = &q->estimations[relId][colId];

		// fprintf(stderr, "%s\n", "::::::::::::::::::::::::::::::::::::::::::::::::::");
		// fprintf(stderr,"%u.%u%c%ld",relId,colId,cmp,constant);
		// fprintf(stderr, "\n%s\n", "::::::::::::::::::::::::::::::::::::::::::::::::::");

		// printColumnStats(&q->estimations[relId][colId]);

		// fprintf(stderr, "\n\n");
		filterEstimation(j,q,colId,stat,actualRelId,relId,cmp,constant);

		// printColumnStats(&q->estimations[relId][colId]);
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
		struct columnStats *statLeft  = &q->estimations[leftRelId][leftColId];
		struct columnStats *statRight = &q->estimations[rightRelId][rightColId];
		struct columnStats *temp;

		// Join between different relations
		if(!isColEquality(&q->predicates[i]))
		{
			// fprintf(stderr,"%u.%u=%u.%u &\n",leftRelId,leftColId,rightRelId,rightColId);

			/* We'll need them for updating every other column of the relations */
			unsigned oldDiscreteLeft  = statLeft->discreteValues;
			unsigned oldDiscreteRight = statRight->discreteValues;

			uint64_t max,min;
			min = (statLeft->minValue > statRight->minValue) ? statLeft->minValue : statRight->minValue;
			max = (statLeft->maxValue < statRight->maxValue) ? statLeft->maxValue : statRight->maxValue;

			// printColumnStats(statLeft);
			// printColumnStats(statRight);

			/* Implicit filter for min value */
			filterEstimation(j,q,leftColId,statLeft,actualRelIdLeft,leftRelId,'>',min);
			filterEstimation(j,q,rightColId,statRight,actualRelIdRight,rightRelId,'>',min);

			/* Implicit filter for max value */
			filterEstimation(j,q,leftColId,statLeft,actualRelIdLeft,leftRelId,'<',max);
			filterEstimation(j,q,rightColId,statRight,actualRelIdRight,rightRelId,'<',max);

			statLeft->f              = statRight->f              = (statLeft->f * statRight->f)/(max-min+1);
			statLeft->minValue       = statRight->minValue       = min;
			statLeft->maxValue       = statRight->maxValue       = max;
			statLeft->discreteValues = statRight->discreteValues = (statLeft->discreteValues*statRight->discreteValues)/(max-min+1);

			/* Update the statistics of every other column of the left relation */
			for (unsigned c = 0; c < (*(j->relations[actualRelIdLeft])).numOfCols; ++c)
			{
				temp = &q->estimations[leftRelId][c];
				if (c != leftColId)
				{
					oldDiscreteLeft = (oldDiscreteLeft == 0) ? 1 : oldDiscreteLeft;
					temp->discreteValues = (temp->discreteValues == 0) ? 1 : temp->discreteValues;

					temp->f = statLeft->f;
					temp->discreteValues =
					temp->discreteValues * (1-power(1-(statLeft->discreteValues/oldDiscreteLeft),(temp->f/temp->discreteValues)));
				}
			}
			/* Update the statistics of every other column of the right relation */
			for (unsigned c = 0; c < (*(j->relations[actualRelIdRight])).numOfCols; ++c)
			{
				temp = &q->estimations[rightRelId][c];
				if (c != rightColId)
				{
					oldDiscreteRight = (oldDiscreteRight == 0) ? 1 : oldDiscreteRight;
					temp->discreteValues = (temp->discreteValues == 0) ? 1 : temp->discreteValues;

					temp->f = statLeft->f;
					temp->discreteValues =
					temp->discreteValues * (1-power(1-(statRight->discreteValues/oldDiscreteRight),(temp->f/temp->discreteValues)));
				}
			}
			// printColumnStats(statLeft);
			// printColumnStats(statRight);
		}
	}
	// fprintf(stderr, "\n");
}

void printColumnStats(struct columnStats *s)
{
	fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	fprintf(stderr, "minValue: %lu\n", s->minValue);
	fprintf(stderr, "maxValue: %lu\n", s->maxValue);
	fprintf(stderr, "f: %u\n", s->f);
	fprintf(stderr, "discreteValues: %u\n", s->discreteValues);
	// fprintf(stderr, "Type of boolean array is: %d\n", s->typeOfBitVector);
	fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
