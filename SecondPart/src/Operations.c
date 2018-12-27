#include <stdio.h>
#include <stdlib.h>/*free()*/
#include <unistd.h>/*sleep() -- debugging*/
#include "Operations.h"

#include "Joiner.h"
#include "Vector.h"
#include "Intermediate.h"
#include "Utils.h"
#include "Partition.h"
#include "Build.h"
#include "Probe.h"

void colEquality(uint64_t *leftCol,uint64_t *rightCol,unsigned numOfTuples,struct Vector **vector)
{	
	/* Create the vector that will hold the results */
	/* 1 stands for: "1 rowId per tuple" -- we do not join relations in this function */
	createVector(vector,1);

	for(unsigned i=0;i<numOfTuples;++i)
		if(leftCol[i]==rightCol[i])
			insertAtVector(*vector,&i);
	// printVector(*vector);
}

void colEqualityInter(uint64_t *leftCol,uint64_t *rightCol,unsigned posLeft,unsigned posRight,struct Vector **vector)
{
	/* Hold the old vector */
	struct Vector *old = *vector;

	/* Create a new vector */
	createVector(vector,getTupleSize(old));

	/* Fill the new one appropriately by scanning the old vector */
	scanColEquality(*vector,old,leftCol,rightCol,posLeft,posRight);
	// printVector(*vector);

	/* Destroy the old */
	destroyVector(&old);
}

void filter(uint64_t *col,Comparison cmp,uint64_t constant,unsigned numOfTuples,struct Vector **vector)
{
	/* Create the vector that will hold the results */
	/* 1 stands for: "1 rowId per tuple" -- we do not join relations in this function */
	createVector(vector,1);
	for(unsigned i=0;i<numOfTuples;++i)
		if(compare(col[i],cmp,constant))
			insertAtVector(*vector,&i);
	// printVector(*vector);
	// 	printf("interResult vector after filter:%u tuples\n",getVectorTuples(*vector));
}

void filterInter(uint64_t *col,Comparison cmp,uint64_t constant,struct Vector **vector)
{
	/* Hold the old vector */
	struct Vector *old = *vector;

	/* Create a new vector */
	createVector(vector,1);

	/* Fill the new one appropriately by scanning the old vector */
	scanFilter(*vector,old,col,cmp,constant);
	// printVector(*vector);

	/* Destroy the old */
	destroyVector(&old);
}

void joinNonInterNonInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right)
{
	// Partition the two columns
	partition(left);
	partition(right);

	// Build index (for the smallest one)
	build(left,right);

	// Probe
	struct Vector *result;
	createVector(&result,left->tupleSize+right->tupleSize);
	probe(left,right,result);
	// printf("Results vector with %u tuples with %u rowIds in each tuple\n",getVectorTuples(result),getTupleSize(result));
	// printVector(result);

	// Update mapRels and interResults //

	// Construct new mapping
	unsigned *newMap = allocate(inter->queryRelations*sizeof(unsigned),"joinNonInterNonInter");
	for(unsigned i=0;i<inter->queryRelations;++i)
		newMap[i] = -1;

	newMap[left->relId]  = 0;
	newMap[right->relId] = 1;
	
	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;
	destroyVector(left->ptrToVec);
	destroyVector(right->ptrToVec);

	// Attach the new ones to first available position 
	unsigned pos             = getFirstAvailablePos(inter);
	inter->mapRels[pos]      = newMap;
	inter->interResults[pos] = result;

	destroyRadixHashJoinInfo(left);
	destroyRadixHashJoinInfo(right);
}

void joinNonInterInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right)
{
	// Partition the two columns
	partition(left);
	partition(right);

	// Build index (for the smallest one)
	build(left,right);

	// Probe
	struct Vector *result;
	createVector(&result,left->tupleSize+right->tupleSize);
	probe(left,right,result);
	// printf("Results vector with %u tuples with %u rowIds in each tuple\n",getVectorTuples(result),getTupleSize(result));
	// printVector(result);

	// Update mapRels and interResults //

	// Construct new mapping
	unsigned *newMap = allocate(inter->queryRelations*sizeof(unsigned),"joinNonInterInter");
	
	newMap[left->relId] = 0;
	for(unsigned i=0;i<inter->queryRelations;++i)
		if(i!=left->relId)
			newMap[i] = (right->map[i]!=-1) ? 1+right->map[i] : -1;

	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;
	destroyVector(left->ptrToVec);
	destroyVector(right->ptrToVec);

	// Attach the new ones to first available position 
	unsigned pos             = getFirstAvailablePos(inter);
	inter->mapRels[pos]      = newMap;
	inter->interResults[pos] = result;
	destroyRadixHashJoinInfo(left);
	destroyRadixHashJoinInfo(right);
}

void joinInterNonInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right)
{
	// Partition the two columns
	partition(left);
	partition(right);

	// Build index (for the smallest one)
	build(left,right);

	// Probe
	struct Vector *result;
	createVector(&result,left->tupleSize+right->tupleSize);
	probe(left,right,result);
	// printf("Results vector with %u tuples with %u rowIds in each tuple\n",getVectorTuples(result),getTupleSize(result));
	// printVector(result);

	// Update mapRels and interResults //

	// Construct new mapping
	unsigned *newMap = allocate(inter->queryRelations*sizeof(unsigned),"joinInterNonInter");

	for(unsigned i=0;i<inter->queryRelations;++i)
		newMap[i] = left->map[i];
	newMap[right->relId] = left->tupleSize;	

	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;
	destroyVector(left->ptrToVec);
	destroyVector(right->ptrToVec);

	// Attach the new ones to first available position 
	unsigned pos             = getFirstAvailablePos(inter);
	inter->mapRels[pos]      = newMap;
	inter->interResults[pos] = result;
	destroyRadixHashJoinInfo(left);
	destroyRadixHashJoinInfo(right);
}

void joinInterInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right)
{
	if(left->vector == right->vector)
	{
		// printf("Join applied as column equality\n");
		unsigned posLeft  = left->map[left->relId];
		unsigned posRight = right->map[right->relId];
		colEqualityInter(left->col,right->col,posLeft,posRight,left->ptrToVec);
		return;
	}
	// Partition the two columns
	partition(left);
	partition(right);

	// Build index (for the smallest one)
	build(left,right);

	// Probe
	struct Vector *result;
	createVector(&result,left->tupleSize+right->tupleSize);
	probe(left,right,result);
	// printf("Results vector with %u tuples with %u rowIds in each tuple\n",getVectorTuples(result),getTupleSize(result));
	// printVector(result);

	// Update mapRels and interResults //

	// Construct new mapping
	unsigned *newMap = allocate(inter->queryRelations*sizeof(unsigned),"joinInterInter");
	for(unsigned i=0;i<inter->queryRelations;++i)
		newMap[i] = left->map[i];

	for(unsigned i=0;i<inter->queryRelations;++i)
		if(newMap[i]==-1)
			newMap[i] = (right->map[i]!=-1) ? right->map[i]+left->tupleSize : -1;

	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;
	destroyVector(left->ptrToVec);
	destroyVector(right->ptrToVec);

	// Attach the new ones to first available position 
	unsigned pos             = getFirstAvailablePos(inter);
	inter->mapRels[pos]      = newMap;
	inter->interResults[pos] = result;
	destroyRadixHashJoinInfo(left);
	destroyRadixHashJoinInfo(right);
}