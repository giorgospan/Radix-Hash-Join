#include <stdio.h>
#include <string.h>/*strlen()*/
#include <unistd.h>/*write()*/
#include <stdlib.h>/*free()*/
#include <pthread.h>

#include "Intermediate.h"
#include "Parser.h"
#include "Partition.h"
#include "Operations.h"
#include "Utils.h"
#include "Vector.h"/* destroyVector(..)*/
#include "Queue.h"
#include "JobScheduler.h"

void createInterMetaData(struct InterMetaData **inter,struct QueryInfo *q)
{
	*inter                    = malloc(sizeof(struct InterMetaData));
	MALLOC_CHECK(*inter);
	(*inter)->maxNumOfVectors = getNumOfFilters(q) + getNumOfColEqualities(q) + getNumOfJoins(q);
	(*inter)->interResults    = malloc((*inter)->maxNumOfVectors*sizeof(struct Vector**));
	MALLOC_CHECK((*inter)->interResults);

	(*inter)->mapRels         = malloc((*inter)->maxNumOfVectors*sizeof(unsigned*));
	MALLOC_CHECK((*inter)->mapRels );
	(*inter)->queryRelations  = getNumOfRelations(q);

	for(unsigned i=0;i<(*inter)->maxNumOfVectors;++i){
		(*inter)->interResults[i] = malloc(HASH_RANGE_1*sizeof(struct Vector*));
		MALLOC_CHECK((*inter)->interResults[i]);
		for(unsigned v=0;v<HASH_RANGE_1;++v)
			(*inter)->interResults[i][v] = NULL;
		(*inter)->mapRels[i] = NULL;
	}
}

void applyColumnEqualities(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q)
{
	for(unsigned i=0;i<q->numOfPredicates;++i)
		if(isColEquality(&q->predicates[i]))
		{

			unsigned original      = getOriginalRelId(q,&q->predicates[i].left);
			unsigned relId         = getRelId(&q->predicates[i].left);
			unsigned leftColId     = getColId(&q->predicates[i].left);
			unsigned rightColId    = getColId(&q->predicates[i].right);
			unsigned pos           = getVectorPos(inter,relId);
			struct Vector **vector = inter->interResults[pos]+0;
			unsigned numOfTuples   = getRelationTuples(joiner,original);
			uint64_t *leftCol      = getColumn(joiner,original,leftColId);
			uint64_t *rightCol     = getColumn(joiner,original,rightColId);
			if(isInInter(vector[0]))
			{
				// fprintf(stderr, "Column Equality Inter\n");
				// printf("~~~Again~~~\n");
				// printf("Pos:%u\n",pos);
				// printf("%u.%u=%u.%u [r%u.tbl]\n",relId,leftColId,relId,rightColId,original);
				colEqualityInter(leftCol,rightCol,0,0,vector);
			}
			else
			{
				// fprintf(stderr, "Column Equality\n");
				// printf("%u.%u=%u.%u [r%u.tbl]\n",relId,leftColId,relId,rightColId,original);
				unsigned *values = malloc(inter->queryRelations*sizeof(unsigned));
				MALLOC_CHECK(values);
				for(unsigned i=0;i<inter->queryRelations;++i)
					values[i] = (i==relId) ? 0 : -1;
				createMap(&inter->mapRels[pos],inter->queryRelations,values);
				free(values);
				colEquality(leftCol,rightCol,numOfTuples,vector);
			}
			// printf("\n\n");
		}
}

void applyFilters(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q)
{
	for(unsigned i=0;i<q->numOfFilters;++i)
	{
		unsigned original      = getOriginalRelId(q,&q->filters[i].filterLhs);
		unsigned relId         = getRelId(&q->filters[i].filterLhs);
		unsigned colId         = getColId(&q->filters[i].filterLhs);
		uint64_t constant      = getConstant(&q->filters[i]);
		Comparison cmp         = getComparison(&q->filters[i]);
		unsigned pos           = getVectorPos(inter,relId);
		struct Vector **vector = inter->interResults[pos];
		unsigned numOfTuples   = getRelationTuples(joiner,original);
		uint64_t *col          = getColumn(joiner,original,colId);
		if(isInInter(vector[0]))
		{
			// fprintf(stderr,"%u.%u%c%lu [r%u.tbl]\n\n",relId,colId,cmp,constant,original);
			filterInter(col,cmp,constant,vector);
		}
		else
		{
			// Create map array [0 in every place except for the relId-th place]
			unsigned *values = malloc(inter->queryRelations*sizeof(unsigned));
			MALLOC_CHECK(values);
			for(unsigned i=0;i<inter->queryRelations;++i)
				values[i] = (i==relId) ? 0 : -1;
			createMap(&inter->mapRels[pos],inter->queryRelations,values);
			free(values);

			// Add filter jobs to the queue
			jobsFinished = 0;
			unsigned chunkSize = numOfTuples / HASH_RANGE_1;
			unsigned lastEnd = 0;
			unsigned i;
			for(i=0;i<HASH_RANGE_1-1;++i){
				struct filterArg *arg = js->filterJobs[i].argument;
				arg->col              = col;
				arg->constant         = constant;
				arg->cmp							= cmp;
				arg->start        		= i*chunkSize;
				arg->end              = arg->start + chunkSize;
				arg->vector           = vector+i;
				lastEnd = arg->end;
				pthread_mutex_lock(&queueMtx);
				enQueue(jobQueue,&js->filterJobs[i]);
				pthread_cond_signal(&condNonEmpty);
				pthread_mutex_unlock(&queueMtx);
			}
			struct filterArg *arg = js->filterJobs[i].argument;
			arg->col              = col;
			arg->constant         = constant;
			arg->cmp							= cmp;
			arg->start        		= lastEnd;
			arg->end              = numOfTuples;
			arg->vector           = vector+i;
			pthread_mutex_lock(&queueMtx);
			enQueue(jobQueue,&js->filterJobs[i]);
			pthread_cond_signal(&condNonEmpty);
			pthread_mutex_unlock(&queueMtx);

			// Wait for all filter jobs to finish
			pthread_mutex_lock(&jobsFinishedMtx);
			while (jobsFinished!=HASH_RANGE_1) {
				pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
			}
			jobsFinished = 0;
			pthread_cond_signal(&condJobsFinished);
			pthread_mutex_unlock(&jobsFinishedMtx);
		}
	}
}

void applyJoins(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q)
{
	for(unsigned i=0;i<q->numOfPredicates;++i)
		if(!isColEquality(&q->predicates[i]))
		{
			RadixHashJoinInfo argLeft,argRight;
			initalizeInfo(inter,q,&q->predicates[i].left,joiner,&argLeft);
			// printf(" = ");
			initalizeInfo(inter,q,&q->predicates[i].right,joiner,&argRight);
			// printf("\n");
			applyProperJoin(inter,&argLeft,&argRight);
			// printf("\n");
		}
}

void applyProperJoin(struct InterMetaData *inter,RadixHashJoinInfo* argLeft,RadixHashJoinInfo* argRight)
{
	switch(argLeft->isInInter){
	case 0:
		switch(argRight->isInInter){
			case 0:
				// fprintf(stderr, "joinNonInterNonInter()\n");
				joinNonInterNonInter(inter,argLeft,argRight);
				break;
			case 1:
				// fprintf(stderr, "joinNonInterInter()\n");
				joinNonInterInter(inter,argLeft,argRight);
				break;
		}
		break;
	case 1:
		switch(argRight->isInInter){
			case 0:
				// fprintf(stderr, "joinInterNonInter()\n");
				joinInterNonInter(inter,argLeft,argRight);
				break;
			case 1:
				// fprintf(stderr, "joinInterInter()\n");
				joinInterInter(inter,argLeft,argRight);
				break;
		}
	}
}

unsigned getVectorPos(struct InterMetaData *inter,unsigned relId)
{
	for(unsigned i=0;i<inter->maxNumOfVectors;++i)
		if(inter->mapRels[i])
			if(inter->mapRels[i][relId]!=-1)
				return i;
	return getFirstAvailablePos(inter);
}

unsigned getFirstAvailablePos(struct InterMetaData* inter)
{
	for(unsigned i=0;i<inter->maxNumOfVectors;++i)
		if(!inter->mapRels[i])
			return i;
}

void createMap(unsigned **mapRels,unsigned size,unsigned *values)
{
	// -1 unsigned = 4294967295 [Hopefully we won't have to deal with so many relations]
	*mapRels = malloc(size*sizeof(unsigned));
	MALLOC_CHECK(*mapRels);
	for(unsigned j=0;j<size;++j)
		(*mapRels)[j] = values[j];
}

unsigned isInInter(struct Vector *vector)
{return vector!=NULL;}

void applyCheckSums(struct InterMetaData *inter,struct Joiner* joiner,struct QueryInfo *q)
{
	for(unsigned i=0;i<q->numOfSelections;++i)
	{
		unsigned original      = getOriginalRelId(q,&q->selections[i]);
		unsigned relId         = getRelId(&q->selections[i]);
		unsigned colId         = getColId(&q->selections[i]);
		struct Vector **vector = inter->interResults[getVectorPos(inter,relId)];
		unsigned *rowIdMap     = inter->mapRels[getVectorPos(inter,relId)];
		uint64_t *col          = getColumn(joiner,original,colId);
		unsigned isLast        = i == q->numOfSelections-1;

		// In case the given relation did not take participate in any of the predicates/filters
		if(!isInInter(vector[0]))
			printCheckSum(0,isLast);
		else
		{
			// Add checkSum jobs to the queue
			for(unsigned i=0;i<HASH_RANGE_1;++i){
				struct checkSumArg *arg = js->checkSumJobs[i].argument;
				arg->vector             = vector[i];
				arg->col                = col;
				arg->rowIdPos           = rowIdMap[relId];
				arg->sum                = js->checkSumArray+i;
				pthread_mutex_lock(&queueMtx);
				enQueue(jobQueue,&js->checkSumJobs[i]);
				pthread_cond_signal(&condNonEmpty);
				pthread_mutex_unlock(&queueMtx);
			}
			// Wait for all checkSum jobs to finish
			pthread_mutex_lock(&jobsFinishedMtx);
			while (jobsFinished!=HASH_RANGE_1) {
				pthread_cond_wait(&condJobsFinished,&jobsFinishedMtx);
			}
			jobsFinished = 0;
			pthread_cond_signal(&condJobsFinished);
			pthread_mutex_unlock(&jobsFinishedMtx);

			// Gather and sum the partial sums
			uint64_t allCheckSums=0;
			for(unsigned i=0;i<HASH_RANGE_1;++i)
				allCheckSums+=js->checkSumArray[i];
			printCheckSum(allCheckSums,isLast);
		}
	}
}

void printCheckSum(uint64_t checkSum,unsigned isLast)
{
	char string[100];
	if(checkSum)
		sprintf(string,"%lu",checkSum);
	else
		sprintf(string,"NULL");

	if(isLast)
	{
		// fprintf(stderr,"%s\n",string);
		printf("%s\n",string);
		fflush(stdout);
	}
	else
	{
		// fprintf(stderr,"%s ",string);
		printf("%s ",string);
	}
}

void initalizeInfo(struct InterMetaData *inter,struct QueryInfo *q,struct SelectInfo *s,struct Joiner *j,RadixHashJoinInfo *arg)
{
	arg->relId          = getRelId(s);
	arg->colId          = getColId(s);
	arg->col            = getColumn(j,getOriginalRelId(q,s),arg->colId);
	arg->vector         = inter->interResults[getVectorPos(inter,arg->relId)];
	arg->map            = inter->mapRels[getVectorPos(inter,arg->relId)];
	arg->queryRelations = inter->queryRelations;
	arg->ptrToVec       = inter->interResults[getVectorPos(inter,arg->relId)]+0;
	arg->ptrToMap       = &inter->mapRels[getVectorPos(inter,arg->relId)];
	arg->pos = getVectorPos(inter,arg->relId);
	if(isInInter(arg->vector[0]))
	{
		arg->isInInter   = 1;
		arg->numOfTuples = 0;
		for(unsigned i=0;i<HASH_RANGE_1;++i)
			if(arg->vector[i])
				arg->numOfTuples += getVectorTuples(arg->vector[i]);
		arg->tupleSize   = getTupleSize(arg->vector[0]);
	}
	else
	{
		arg->isInInter   = 0;
		arg->numOfTuples = getRelationTuples(j,getOriginalRelId(q,s));
		arg->tupleSize   = 1;
	}
}


void destroyInterMetaData(struct InterMetaData *inter)
{
	for(unsigned i=0;i<inter->maxNumOfVectors;++i)
	{
		for(unsigned v=0;v<HASH_RANGE_1;++v)
			destroyVector(inter->interResults[i]+v);
		free(inter->interResults[i]);
		free(inter->mapRels[i]);
	}
	free(inter->interResults);
	free(inter->mapRels);
	free(inter);
}


void destroyRadixHashJoinInfo(RadixHashJoinInfo *info)
{
	destroyColumnInfo(&info->unsorted);
	destroyColumnInfo(&info->sorted);
	free(info->hist);
	free(info->pSum);

	/* For every bucket of the relation */
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		/* If we've created an array of indexes for this relation */
		if(info->indexArray)
			/* If this bucket has an index */
			if(info->indexArray[i] != NULL)
			{
				/* Free index fields */
				free(info->indexArray[i]->chainArray);
				free(info->indexArray[i]->bucketArray);

				/* Free index struct itself */
				free(info->indexArray[i]);
			}
	free(info->indexArray);
}

void destroyColumnInfo(ColumnInfo **c)
{
	if(*c)
	{
		free((*c)->values);
		free((*c)->rowIds);
		destroyVector(&(*c)->tuples);
	}
	free(*c);
	*c = NULL;
}
