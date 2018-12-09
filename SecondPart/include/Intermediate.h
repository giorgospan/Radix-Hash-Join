#ifndef INTERMEDIATE_H
#define INTERMEDIATE_H

#include "Joiner.h"
#include "Parser.h"

struct InterMetaData
{
	/* Array of vectors: Each vector will be an "intermediate" entity */
	struct Vector **interResults;
	/**
	 * Array of arrays: One array per interResult vector.
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

/**
 * @brief      Holds info about the relation we are joining
 */
typedef struct JoinArg
{
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
	struct Vector *vector;

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

}JoinArg;

/* Creators/Initializers */
void createInterMetaData(struct InterMetaData **inter,struct QueryInfo *q);
void initalizeJoinArg(struct InterMetaData *inter,struct QueryInfo *q,struct SelectInfo *s,struct Joiner *j,JoinArg *arg);

/* Apply Functions */
void applyColumnEqualities(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);
void applyFilters(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);
void applyJoins(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q);
void applyProperJoin(struct InterMetaData *inter,JoinArg* argLeft,JoinArg* argRight);
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

#endif