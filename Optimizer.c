#include <stdio.h>
#include <stdlib.h>
#include "Optimizer.h"
#include "Utils.h"
#include "Parser.h"
#include "Joiner.h"
#include "Relation.h"


struct columnStats** allocateStatArray(uint64_t size)
{
	struct columnStats** temp = malloc(sizeof(struct columnStats*) * size);
	MALLOC_CHECK(temp);

	for (uint64_t i = 0; i < size; i++)
	{
		temp[i] = malloc(sizeof(struct columnStats));
		MALLOC_CHECK(temp[i]);
		temp[i]->booleanArray = NULL;
	}
	return temp;
}

void deAllocateStatArray(struct columnStats **array, uint64_t size)
{
	for (uint64_t i = 0; i < size; i++)
	{
		free(array[i]->booleanArray);
		free(array[i]);
	}
	free(array);
}

void columnPrint(uint64_t *column, uint64_t columnSize)
{
	fprintf(stderr, "~~\n");
	for (uint64_t i = 0; i < columnSize; i++)
		fprintf(stderr, "%lu\n", column[i]);
	fprintf(stderr, "~~\n");
}

void findStats(uint64_t *column, struct columnStats *stat, uint64_t columnSize)
{
	/* Find MIN and MAX */
	uint64_t min = column[0];
	uint64_t max = column[0];
	for (uint64_t i = 1; i < columnSize; ++i)
	{
		if (column[i] > max)
			max = column[i];
		if (column[i] < min)
			min = column[i];
	}

	/* Find discrete values */
	uint64_t sizeOfBooleanArray = (max - min + 1 > PRIMELIMIT) ? PRIMELIMIT : max - min + 1;
	// fprintf(stderr, "Size of boolean array is  %lu\n", sizeOfBooleanArray);

	stat->booleanArray = malloc(sizeOfBooleanArray);
	MALLOC_CHECK(stat->booleanArray);
	stat->sizeOfBooleanArray = sizeOfBooleanArray;

	/* Simple init for valgrind */
	for (uint64_t i = 0; i < sizeOfBooleanArray; ++i)
			stat->booleanArray[i] = 0;
	/* */

	/* Find the way to fill the boolean array depending on it's size relative to PRIMELIMIT */
	if (sizeOfBooleanArray != PRIMELIMIT)
	{
		stat->typeOfBooleanArray = 0;
		for (uint64_t i = 0; i < columnSize; ++i)
		{
			if (stat->booleanArray[column[i] - min] == 0)
				(stat->discreteValues)++;
			stat->booleanArray[column[i] - min] = 1;
		}
	}
	else
	{
		stat->typeOfBooleanArray = 1;
		for (uint64_t i = 0; i < columnSize; ++i)
		{
			if (stat->booleanArray[(column[i] - min) % PRIMELIMIT] == 0)
				(stat->discreteValues)++;
			stat->booleanArray[(column[i] - min) % PRIMELIMIT] = 1;
		}
	}

	/* Assign the remaining values to the stats*/
	stat->minValue = min;
	stat->maxValue = max;
	stat->f = columnSize;
	// printBooleanArray(stat->booleanArray, sizeOfBooleanArray);
	// printColumnStats(stat);
}

void printBooleanArray(char *array, uint64_t size)
{
	uint64_t trueCounter = 0;
	uint64_t falseCounter = 0;

	for (uint64_t i = 0; i < size; ++i)
	{
		if (array[i] == 1)
		{
			trueCounter++;
			// fprintf(stderr, "True\n");
		}
		else
		{
			falseCounter++;
			// fprintf(stderr, "False\n");
		}
	}
	fprintf(stderr, "TRUE: %lu FALSE: %lu\n", trueCounter, falseCounter);
}

void printColumnStats(struct columnStats *s)
{
	fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
	fprintf(stderr, "minValue: %lu\n", s->minValue);
	fprintf(stderr, "maxValue: %lu\n", s->maxValue);
	fprintf(stderr, "f: %lu\n", s->f);
	fprintf(stderr, "discreteValues: %lu\n", s->discreteValues);
	printBooleanArray(s->booleanArray, s->sizeOfBooleanArray);
	fprintf(stderr, "Size of boolean array is: %lu\n", s->sizeOfBooleanArray);
	fprintf(stderr, "Type of boolean array is: %d\n", s->typeOfBooleanArray);
	fprintf(stderr, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}


void applyFilterEstimations(struct QueryInfo *q, struct Joiner *jo)
{
	// fprintf(stderr, "Num of relations %u\n", jo->numOfRelations);
	fprintf(stderr, "Num of filters %u\n", q->numOfFilters);
	// fprintf(stderr, "Num of selections %u\n", q->numOfSelections);
	// fprintf(stderr, "%p\n", jo->*(relations));
	fprintf(stderr, "Num of numOfTuples %u\n", jo->relations[0]->numOfTuples);
	fprintf(stderr, "Num of columns %u\n", jo->relations[0]->numOfCols);
	// fprintf(stderr, "q->numOfFilters %u\n", q->numOfFilters);

	// for(unsigned jo=0;jo<q->numOfSelections;++jo)
	// {
	// 	unsigned relId = getRelId(&q->selections[jo]);
	// 	unsigned colId = getColId(&q->selections[jo]);
	// 	fprintf(stderr,"%u.%u ",relId,colId);
	// 	fprintf(stderr, "getOriginalRelId %u\n", getOriginalRelId(q, &q->selections[jo]));
	// }
	// fprintf(stderr, "\n");

	for(unsigned i = 0 ; i < q->numOfFilters; ++i)
	{
		unsigned relId    = getRelId(&q->filters[i].filterLhs);
		unsigned colId    = getColId(&q->filters[i].filterLhs);
		Comparison cmp    = getComparison(&q->filters[i]);
		uint64_t constant = getConstant(&q->filters[i]);

		fprintf(stderr, "%s\n", "::::::::::::::::::::::::::::::::::::::::::::::::::");
		fprintf(stderr,"%u.%u%c%ld",relId,colId,cmp,constant);
		fprintf(stderr, "\n%s\n", "::::::::::::::::::::::::::::::::::::::::::::::::::");
		fprintf(stderr, "colId %u\n", colId);

		/* New variables needed */
		uint64_t actualIdOfRelation = getOriginalRelId(q, &q->filters[i].filterLhs);;
		fprintf(stderr, "actualIdOfRelation %lu\n", actualIdOfRelation);
		struct columnStats *addresOfColumnStat = (*(jo->relations[actualIdOfRelation])).colStats[colId];
		struct columnStats *tempAddresOfColumnStat;
		uint64_t fTemp = addresOfColumnStat->f;
		char isInArray;

		int flag = 0;
		if (cmp == '=')
		{

			// printRelation(jo->relations[relId]);
			printColumnStats( (*(jo->relations[actualIdOfRelation])).colStats[colId] );

			/* Find if constant is in the discrete values */
			/* If constant is not in the range of values of the column we won't find it in the booleanArray */
			if ( (constant < addresOfColumnStat->minValue) ||  (constant > addresOfColumnStat->maxValue) )
				isInArray = 0;
			/* If the value in the boolean array is 1 then it is in the booleanArray */
			else
			{
				/* Make sure we use the correct way to find if it is in the booleanArray */
				if (addresOfColumnStat->typeOfBooleanArray == 0)
				{
					if (addresOfColumnStat->booleanArray[constant - addresOfColumnStat->minValue] == 1)
						isInArray = 1;
				}
				else if (addresOfColumnStat->typeOfBooleanArray == 1)
				{
					if (addresOfColumnStat->booleanArray[(constant - addresOfColumnStat->minValue) % PRIMELIMIT] == 1)
						isInArray = 1;
				}
			}

			/* Change the statistics  of the column */
			addresOfColumnStat->minValue = constant;
			addresOfColumnStat->maxValue = constant;
			if (isInArray == 0)
			{
				addresOfColumnStat->f = 0;
				addresOfColumnStat->discreteValues = 0;
			}
			else
			{
				addresOfColumnStat->f = addresOfColumnStat->f / addresOfColumnStat->discreteValues;
				addresOfColumnStat->discreteValues = 1;
			}
			printColumnStats(addresOfColumnStat);

			// printColumnStats( (*(jo->relations[actualIdOfRelation])).colStats[colId] );
			// printColumnStats( (*(jo->relations[relId])).colStats[2] );
			// printRelation(*(jo->relations[relId]));
		}
		else
		{
			flag = 1;
			fprintf(stderr, "%s\n", "COMPAREEEEEEEEEEEEEEEEEEEEEEEEEEEE");
			/* cmp is '>' or '<' */
			/* lowerLimit */
			uint64_t k1 = (cmp == '>') ?  constant - 1 : addresOfColumnStat->minValue - 1;
			/* upperLimit */
			uint64_t k2 = (cmp == '<') ?  constant + 1 : addresOfColumnStat->maxValue + 1;

			if (k1 < addresOfColumnStat->minValue)
				k1 = addresOfColumnStat->minValue - 1;

			if (k2 > addresOfColumnStat->maxValue)
				k2 = addresOfColumnStat->maxValue + 1;

			/* Change the statistics  of the column */
			addresOfColumnStat->minValue = k1;
			addresOfColumnStat->maxValue = k2;
			addresOfColumnStat->discreteValues = ((k2 - k1) / (addresOfColumnStat->maxValue - addresOfColumnStat->minValue))
			 * addresOfColumnStat->discreteValues;
			addresOfColumnStat->f = ((k2 - k1) / (addresOfColumnStat->maxValue - addresOfColumnStat->minValue))
			 * addresOfColumnStat->f;

			printColumnStats(addresOfColumnStat);
			/* REMOVE FOR FLOATING POINT EXCEPTION */
			break;


		}

		// if ( flag == 1) return;


		/* Update the statistics of every other column
			The formulas are the same for every filter */
		for (uint64_t j = 0; j < (*(jo->relations[actualIdOfRelation])).numOfCols; ++j)
		{
			// fprintf(stderr, "fTemp %lu\n", fTemp);
			fprintf(stderr, "%u\n", (*(jo->relations[actualIdOfRelation])).numOfCols);
			fprintf(stderr, "%u\n", q->relationIds[j]);
			tempAddresOfColumnStat = (*(jo->relations[actualIdOfRelation])).colStats[j];
			/* Make sure we update every other column , expect the one that we applied the filter stats */
			if (j != colId)
			{
				fprintf(stderr, "%s\n", "\n*********** UPDATE *********** \n");
				printColumnStats(tempAddresOfColumnStat);

				fprintf(stderr, "%lu\n", tempAddresOfColumnStat->discreteValues * (1 -
					power(1 - (1 - addresOfColumnStat->f / fTemp),
					 tempAddresOfColumnStat->f / tempAddresOfColumnStat->discreteValues)));

				fprintf(stderr, "%lu\n", tempAddresOfColumnStat->discreteValues);

				tempAddresOfColumnStat->discreteValues = tempAddresOfColumnStat->discreteValues * (1 -
					power(1 - (1 - addresOfColumnStat->f / fTemp),
					 tempAddresOfColumnStat->f / tempAddresOfColumnStat->discreteValues));
				tempAddresOfColumnStat->f = addresOfColumnStat->f;
				// fprintf(stderr, "%s\n", "EDWWWWWWWWWWWWWWWWWWWWWWWWWW EIMAIIIIIIIIIII" );
				printColumnStats(tempAddresOfColumnStat);
			}
			// else
			// 	printColumnStats(tempAddresOfColumnStat);
		}


	}
	fprintf(stderr, "\n");



}

void applyPredicates(struct QueryInfo *q, struct Joiner *jo)
{

	/* Find stats for columns in predicates */
	for(unsigned j=0;j<q->numOfPredicates;++j)
	{
		fprintf(stderr, "%s\n", "\nHELLO");
		unsigned leftRelId  = getRelId(&q->predicates[j].left);
		unsigned rightRelId = getRelId(&q->predicates[j].right);
		unsigned leftColId  = getColId(&q->predicates[j].left);
		unsigned rightColId = getColId(&q->predicates[j].right);

		if(!isColEquality(&q->predicates[j]))
		{
			fprintf(stderr,"%u.%u=%u.%u & ",leftRelId,leftColId,rightRelId,rightColId);
			fprintf(stderr, "%s\n", "EDDDDDWWWWWWWWWWSSSSSSSSSSSSSSSSSSSSSSSSSSSS");
		}
		else
			fprintf(stderr,"[%u.%u=%u.%u] & ",leftRelId,leftColId,rightRelId,rightColId);
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
