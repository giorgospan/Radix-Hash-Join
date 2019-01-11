#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "Joiner.h"
#include "Parser.h"

struct InterMetaData
{
	/**
	 * Array arrays of vectors: Each vector-array will be an "intermediate" entity
	 * We need one array of vectors per "intermediate" entity because we'll be computing
	 * the intermediate result using many jobs, not just one. Those jobs will be served
	 * by our threads.
	 */
	struct Vector ***interResults;

	/**
	 * Array of arrays: One array per interResult .
	 * Each array will be of size "queryRelations"
	 * It is actually a  mapping between a relation and where its rowIds are placed inside the tuple
	 * E.g: mapRels[0][1] = 2    means: 0-th vector will contain tuples of the following format:
	 *
	 * <rowIdX,rowIdY,rowId1,...> , where X,Y are ids of the joined relations
	 * 0-th rowId inside tuple is from relation X,
	 * 1-st rowId inside tuple is from relation Y,
	 * 2-nd rowId inside tuple is from relation 1,
	 * e.t.c
	 *
	 * In general : mapRels[..][relId] = tupleOffset
	 */
	unsigned **mapRels;

	/* Number of relations participating in the query */
	unsigned queryRelations;

	/* Size of interResults array
	 * I.e: max number of "intermediate" entities that might be created
	 */
	unsigned maxNumOfVectors;
};

typedef struct ColumnInfo
{
	uint64_t *values;
	unsigned *rowIds;
	struct Vector *tuples;
}ColumnInfo;

typedef struct Index
{
	/* These are the two arrays
		used for the indexing of a single bucket */
	unsigned *chainArray;
	unsigned *bucketArray;
}Index;

/**
 * @brief      Holds info about the relation we are joining
 */
typedef struct RadixHashJoinInfo
{
	unsigned pos;
	// Id of the relation [relevant to the parse order]
	unsigned relId;

	// Id of the column
	unsigned colId;

	// Column values
	uint64_t *col;

	// Number of tuples [either in original relation or in intermediate vector]
	unsigned numOfTuples;

	// Number of rowIds in each tuple of the vector
	// In case the relation is not in the intermediate results,tupleSize won't be used at all.
	unsigned tupleSize;

	// Intermediate vector
	// In case the relation is not in the intermediate results,vector won't be used at all.
	struct Vector **vector;

	// Vector's address [Useful when we want to destroy the vector after having executed the join]
	struct Vector **ptrToVec;

	// Map[relation <---> vector's tuple]
	// In case the relation is not in the intermediate results,map won't be used at all.
	unsigned *map;

	// Map's address [Useful when we want to destroy the map array after having executed the join]
	unsigned **ptrToMap;

	// Number of relations participating in the query
	// I.e: size of map array
	unsigned queryRelations;

	// 1 if relation is in interResults
	// 0 otherwise
	unsigned isInInter;

	// Will be set to 1 during build phase, in case this is the small column
	// between the two columns we're joining.
	unsigned isSmall;
	// 1: if it is on join's lhs, 0 otherwise
	unsigned isLeft;

	ColumnInfo *unsorted;
	ColumnInfo *sorted;
	unsigned *hist;
	unsigned *pSum;
	Index **indexArray;
}RadixHashJoinInfo;

/* Creators/Initializers */
void createInterMetaData(struct InterMetaData **inter,struct QueryInfo *q);
void initalizeInfo(struct InterMetaData *inter,struct QueryInfo *q,struct SelectInfo *s,struct Joiner *j,RadixHashJoinInfo *arg);

/* Apply Functions */
void applyColumnEqualities(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);
void applyFilters(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);
void applyJoins(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);
void applyProperJoin(struct InterMetaData *inter,RadixHashJoinInfo* argLeft,RadixHashJoinInfo* argRight);
void applyCheckSums(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);

/* Check functions */
unsigned isInInter(struct Vector *vector);

/* Misc */
unsigned getVectorPos(struct InterMetaData *inter,unsigned relId);
unsigned getFirstAvailablePos(struct InterMetaData* inter);
void createMap(unsigned **mapRels,unsigned size,unsigned *values);
void printCheckSum(uint64_t checkSum,unsigned iSLast);

/* Destroyer */
void destroyInterMetaData(struct InterMetaData *inter);
void destroyRadixHashJoinInfo(RadixHashJoinInfo *);
void destroyColumnInfo(ColumnInfo **c);

#endif
