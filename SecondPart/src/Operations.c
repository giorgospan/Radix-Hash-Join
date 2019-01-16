#include <stdio.h>
#include <stdlib.h>/*free()*/
#include <unistd.h>/*sleep() -- debugging*/
#include <pthread.h>
#include "Operations.h"

#include "Joiner.h"
#include "Vector.h"
#include "Intermediate.h"
#include "Utils.h"
#include "Partition.h"
#include "Build.h"
#include "Probe.h"
#include "JobScheduler.h"
#include "Queue.h"

void colEquality(uint64_t *leftCol,uint64_t *rightCol,unsigned numOfTuples,struct Vector **vector)
{
	/* Create the vector that will hold the results */
	/* 1 stands for: "1 rowId per tuple" -- we do not join relations in this function */
	createVector(vector,1);

	for(unsigned i=0;i<numOfTuples;++i)
		if(leftCol[i]==rightCol[i])
			insertAtVector(*vector,&i);
}

void colEqualityInter(uint64_t *leftCol,uint64_t *rightCol,unsigned posLeft,unsigned posRight,struct Vector **vector)
{
	struct Vector **results;
	jobsFinished=0;
	results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
	MALLOC_CHECK(results);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		createVector(results+i,getTupleSize(vector[0]));

	for(unsigned i=0;i<HASH_RANGE_1;++i){
		struct colEqualityArg *arg = js->colEqualityJobs[i].argument;
		arg->new                   = results[i];
		arg->old                   = vector[i];
		arg->leftCol               = leftCol;
		arg->rightCol              = rightCol;
		arg->posLeft               = posLeft;
		arg->posRight              = posRight;
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->colEqualityJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
	}
	pthread_mutex_lock(&jobsFinishedMtx);
	while (jobsFinished!=HASH_RANGE_1) {
		pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
	}
	jobsFinished = 0;
	pthread_cond_signal(&condJobsFinished);
	pthread_mutex_unlock(&jobsFinishedMtx);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		destroyVector(vector+i);

	for(unsigned i=0;i<HASH_RANGE_1;++i){
		vector[i] = results[i];
	}
	free(results);
}

void filter(uint64_t *col,Comparison cmp,uint64_t constant,unsigned numOfTuples,struct Vector **vector)
{
	/* Create the vector that will hold the results */
	/* 1 stands for: "1 rowId per tuple" -- we do not join relations in this function */
	createVector(vector,1);
	for(unsigned i=0;i<numOfTuples;++i)
		if(compare(col[i],cmp,constant))
			insertAtVector(*vector,&i);
}

void filterFunc(void *arg){
	struct filterArg *myarg = arg;
	createVector(myarg->vector,1);
	for(unsigned i=myarg->start;i<myarg->end;++i)
		if(compare(myarg->col[i],myarg->cmp,myarg->constant))
			insertAtVector(*myarg->vector,&i);
	pthread_mutex_lock(&jobsFinishedMtx);
	++jobsFinished;
	pthread_cond_signal(&condJobsFinished);
	pthread_mutex_unlock(&jobsFinishedMtx);
}

void filterInter(uint64_t *col,Comparison cmp,uint64_t constant,struct Vector **vector)
{
	/* Hold the old vector */
	struct Vector *old = *vector;

	/* Create a new vector */
	createVector(vector,1);

	/* Fill the new one appropriately by scanning the old vector */
	scanFilter(*vector,old,col,cmp,constant);

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
	left->isLeft  = 1;
	right->isLeft = 0;

	// Probe
	struct Vector **results;
	results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
	MALLOC_CHECK(results);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		createVector(results+i,left->tupleSize+right->tupleSize);

	for(unsigned i=0;i<HASH_RANGE_1;++i){
		struct joinArg *arg = js->joinJobs[i].argument;
		arg->bucket         = i;
		arg->left           = left;
		arg->right          = right;
		arg->results        = results[i];
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->joinJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
	}
	pthread_mutex_lock(&jobsFinishedMtx);
	while (jobsFinished!=HASH_RANGE_1) {
		pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
	}
	jobsFinished = 0;
	pthread_cond_signal(&condJobsFinished);
	pthread_mutex_unlock(&jobsFinishedMtx);

	// Update mapRels and interResults //
	// Construct new mapping
	unsigned *newMap = malloc(inter->queryRelations*sizeof(unsigned));
	MALLOC_CHECK(newMap);
	for(unsigned i=0;i<inter->queryRelations;++i)
		newMap[i] = -1;

	newMap[left->relId]  = 0;
	newMap[right->relId] = 1;

	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;

	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		destroyVector(left->ptrToVec+i);
		destroyVector(right->ptrToVec+i);
	}

	// Attach the new ones to first available position
	unsigned pos                = getFirstAvailablePos(inter);
	inter->mapRels[pos]         = newMap;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		inter->interResults[pos][i] = results[i];

	free(results);
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
	left->isLeft  = 1;
	right->isLeft = 0;

	// Probe
	jobsFinished=0;
	struct Vector **results;
	results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
	MALLOC_CHECK(results);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		createVector(results+i,left->tupleSize+right->tupleSize);

	for(unsigned i=0;i<HASH_RANGE_1;++i){
		struct joinArg *arg = js->joinJobs[i].argument;
		arg->bucket         = i;
		arg->left           = left;
		arg->right          = right;
		arg->results        = results[i];
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->joinJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
	}
	pthread_mutex_lock(&jobsFinishedMtx);
	while (jobsFinished!=HASH_RANGE_1) {
		pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
	}
	jobsFinished = 0;
	pthread_cond_signal(&condJobsFinished);
	pthread_mutex_unlock(&jobsFinishedMtx);

	// Update mapRels and interResults //
	// Construct new mapping
	unsigned *newMap = malloc(inter->queryRelations*sizeof(unsigned));
	MALLOC_CHECK(newMap);

	newMap[left->relId] = 0;
	for(unsigned i=0;i<inter->queryRelations;++i)
		if(i!=left->relId)
			newMap[i] = (right->map[i]!=-1) ? 1+right->map[i] : -1;

	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		destroyVector(left->ptrToVec+i);
		destroyVector(right->ptrToVec+i);
	}

	// Attach the new ones to first available position
	unsigned pos                = getFirstAvailablePos(inter);
	inter->mapRels[pos]         = newMap;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		inter->interResults[pos][i] = results[i];

	free(results);
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
	left->isLeft  = 1;
	right->isLeft = 0;

	// Probe
	struct Vector **results;
	jobsFinished = 0;
	results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
	MALLOC_CHECK(results);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		createVector(results+i,left->tupleSize+right->tupleSize);

	for(unsigned i=0;i<HASH_RANGE_1;++i){
		struct joinArg *arg = js->joinJobs[i].argument;
		arg->bucket         = i;
		arg->left           = left;
		arg->right          = right;
		arg->results        = results[i];
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->joinJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
	}
	pthread_mutex_lock(&jobsFinishedMtx);
	while (jobsFinished!=HASH_RANGE_1) {
		pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
	}
	jobsFinished = 0;
	pthread_cond_signal(&condJobsFinished);
	pthread_mutex_unlock(&jobsFinishedMtx);

	// Update mapRels and interResults //
	// Construct new mapping
	unsigned *newMap = malloc(inter->queryRelations*sizeof(unsigned));
	MALLOC_CHECK(newMap);

	for(unsigned i=0;i<inter->queryRelations;++i)
		newMap[i] = left->map[i];
	newMap[right->relId] = left->tupleSize;

	// Free the old map arrays | Destroy the old vectors
	free(*left->ptrToMap);
	*left->ptrToMap = NULL;
	free(*right->ptrToMap);
	*right->ptrToMap = NULL;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		destroyVector(left->ptrToVec+i);
		destroyVector(right->ptrToVec+i);
	}

	// Attach the new ones to first available position
	unsigned pos                = getFirstAvailablePos(inter);
	inter->mapRels[pos]         = newMap;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		inter->interResults[pos][i] = results[i];

	free(results);
	destroyRadixHashJoinInfo(left);
	destroyRadixHashJoinInfo(right);
}

void joinInterInter(struct InterMetaData *inter,RadixHashJoinInfo* left,RadixHashJoinInfo* right)
{
	if(left->vector == right->vector)
	{
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
	left->isLeft  = 1;
	right->isLeft = 0;

	// Probe
	struct Vector **results;
	jobsFinished=0;
	results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
	MALLOC_CHECK(results);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		createVector(results+i,left->tupleSize+right->tupleSize);

	for(unsigned i=0;i<HASH_RANGE_1;++i){
		struct joinArg *arg = js->joinJobs[i].argument;
		arg->bucket         = i;
		arg->left           = left;
		arg->right          = right;
		arg->results        = results[i];
		pthread_mutex_lock(&queueMtx);
		enQueue(jobQueue,&js->joinJobs[i]);
		pthread_cond_signal(&condNonEmpty);
		pthread_mutex_unlock(&queueMtx);
	}
	pthread_mutex_lock(&jobsFinishedMtx);
	while (jobsFinished!=HASH_RANGE_1) {
		pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
	}
	jobsFinished = 0;
	pthread_cond_signal(&condJobsFinished);
	pthread_mutex_unlock(&jobsFinishedMtx);

	// Update mapRels and interResults //
	// Construct new mapping
	unsigned *newMap = malloc(inter->queryRelations*sizeof(unsigned));
	MALLOC_CHECK(newMap);
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
	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		destroyVector(left->ptrToVec+i);
		destroyVector(right->ptrToVec+i);
	}

	// Attach the new ones to first available position
	unsigned pos                = getFirstAvailablePos(inter);
	inter->mapRels[pos]         = newMap;
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		inter->interResults[pos][i] = results[i];

	free(results);
	destroyRadixHashJoinInfo(left);
	destroyRadixHashJoinInfo(right);
}
