#include <stdio.h>
#include <stdlib.h>
#include <string.h> /*strtok(),strcpy()*/

#include "Parser.h"
#include "Utils.h"
#include "Relation.h"
#include "Optimizer.h"


void createQueryEstimations(struct QueryInfo *qInfo,struct Joiner *j)
{
	qInfo->estimations = malloc(qInfo->numOfRelationIds*sizeof(struct columnStats*));
	MALLOC_CHECK(qInfo->estimations);
	for(unsigned i=0;i<qInfo->numOfRelationIds;++i)
	{
		unsigned relId = qInfo->relationIds[i];
		unsigned size = j->relations[relId]->numOfCols*sizeof(struct columnStats);

		// Allocate space to store the estimations
		qInfo->estimations[i] = malloc(size);
		MALLOC_CHECK(qInfo->estimations[i]);
		// Fetch the stats, calculated when the relations were being loaded in memory
		memcpy(qInfo->estimations[i],j->relations[relId]->stats,size);
	}
}

void createQueryInfo(struct QueryInfo **qInfo,char *rawQuery)
{
	*qInfo = malloc(sizeof(struct QueryInfo));
	MALLOC_CHECK(*qInfo);
	parseQuery(*qInfo,rawQuery);
}

void destroyQueryInfo(struct QueryInfo *qInfo)
{
	free(qInfo->relationIds);
	free(qInfo->predicates);
	free(qInfo->filters);
	free(qInfo->selections);
	for(unsigned i=0;i<qInfo->numOfRelationIds;++i)
		free(qInfo->estimations[i]);
	free(qInfo->estimations);
	free(qInfo);
}

void parseQuery(struct QueryInfo *qInfo,char *rawQuery)
{
	char rawRelations[BUFFERSIZE];
	char rawPredicates[BUFFERSIZE];
	char rawSelections[BUFFERSIZE];

	/* Split query into three parts */
	if( (sscanf(rawQuery,"%[^|]|%[^|]|%[^|]",rawRelations,rawPredicates,rawSelections)) != 3 )
	{
		fprintf(stderr,"Query \"%s\" does not consist of three parts\nExiting...\n\n",rawQuery);
		exit(EXIT_FAILURE);
	}

	/* Parse each part */
	parseRelationIds(qInfo,rawRelations);
	parsePredicates(qInfo,rawPredicates);
	parseSelections(qInfo,rawSelections);
}

void parseRelationIds(struct QueryInfo *qInfo,char *rawRelations)
{
	char* temp = rawRelations;
	unsigned i;
	int offset;

	/* Get number of relationIds */
	qInfo->numOfRelationIds = 0;
	while(sscanf(temp,"%*d%n",&offset)>=0)
	{
		++qInfo->numOfRelationIds;
		temp+=offset;
	}
	if(!qInfo->numOfRelationIds)
	{
		fprintf(stderr,"Zero join relations were found in the query\nExiting...\n\n");
		exit(EXIT_FAILURE);
	}

	/* Allocate memory for relationIds */
	qInfo->relationIds = malloc(qInfo->numOfRelationIds*sizeof(unsigned));
	MALLOC_CHECK(qInfo->relationIds);

	/* Store relationIds */
	temp = rawRelations;
	i=0;
	while(sscanf(temp,"%u%n",&qInfo->relationIds[i],&offset)>0)
	{
		++i;
		temp+=offset;
	}
}

void parseSelections(struct QueryInfo *qInfo,char *rawSelections)
{

	char* temp = rawSelections;
	unsigned relId,colId,i;
	int offset;

	/* Get number of selections */
	qInfo->numOfSelections = 0;
	while(sscanf(temp,"%*u.%*u%n",&offset)>=0)
	{
		++qInfo->numOfSelections;
		temp+=offset;
	}
	if(!qInfo->numOfSelections)
	{
		fprintf(stderr,"Zero selections were found in the query\nExiting...\n\n");
		exit(EXIT_FAILURE);
	}

	/* Allocate memory for selections */
	qInfo->selections = malloc(qInfo->numOfSelections*sizeof(struct SelectInfo));
	MALLOC_CHECK(qInfo->selections);

	/*  Store selections */
	temp = rawSelections;
	i=0;
	while(sscanf(temp,"%u.%u%n",&relId,&colId,&offset)>0)
	{
		qInfo->selections[i].relId = relId;
		qInfo->selections[i].colId = colId;
		++i;
		temp+=offset;
	}
}

void parsePredicates(struct QueryInfo *qInfo,char *rawPredicates)
{
	unsigned i,j;
	char *token;
	char *temp = malloc((strlen(rawPredicates)+1)*sizeof(char));
	MALLOC_CHECK(temp);
	strcpy(temp,rawPredicates);

	/* Get number of predicates and filters */
	qInfo->numOfFilters    = 0;
	qInfo->numOfPredicates = 0;
	token = strtok(temp,"&");
	if(isFilter(token))
		++qInfo->numOfFilters;
	else
		++qInfo->numOfPredicates;
	while(token=strtok(NULL,"&"))
		if(isFilter(token))
			++qInfo->numOfFilters;
		else
			++qInfo->numOfPredicates;

	if(!(qInfo->numOfPredicates+qInfo->numOfFilters))
	{
		fprintf(stderr,"Zero predicates were found in the query\nExiting...\n\n");
		exit(EXIT_FAILURE);
	}

	/* Allocate memory for predicates and filters */
	qInfo->predicates = malloc(qInfo->numOfPredicates*sizeof(struct PredicateInfo));
	MALLOC_CHECK(qInfo->predicates);
	qInfo->filters    = malloc(qInfo->numOfFilters*sizeof(struct FilterInfo));
	MALLOC_CHECK(qInfo->filters);

	/* Store predicates & filters */
	strcpy(temp,rawPredicates);
	token = strtok(temp,"&");
	i=j=0;
	if(isFilter(token))
		{addFilter(&qInfo->filters[i],token);++i;}
	else
		{addPredicate(&qInfo->predicates[j],token);++j;}


	while(token=strtok(NULL,"&"))
		if(isFilter(token))
			{addFilter(&qInfo->filters[i],token);++i;}
		else
			{addPredicate(&qInfo->predicates[j],token);++j;}

	free(temp);
}

void addFilter(struct FilterInfo *fInfo,char *token)
{
	unsigned relId;
	unsigned colId;
	char cmp;
	uint64_t constant;
	sscanf(token,"%u.%u%c%lu",&relId,&colId,&cmp,&constant);
	// printf("\"%u.%u%c%lu\"\n",relId,colId,cmp,constant);
	fInfo->filterLhs.relId = relId;
	fInfo->filterLhs.colId = colId;
	fInfo->comparison      = cmp;
	fInfo->constant        = constant;
}

void addPredicate(struct PredicateInfo *pInfo,char *token)
{
	unsigned relId1;
	unsigned colId1;
	unsigned relId2;
	unsigned colId2;
	sscanf(token,"%u.%u=%u.%u",&relId1,&colId1,&relId2,&colId2);
	// printf("\"%u.%u=%u.%u\"\n",relId1,colId1,relId2,colId2);
	pInfo->left.relId  = relId1;
	pInfo->left.colId  = colId1;
	pInfo->right.relId = relId2;
	pInfo->right.colId = colId2;
}

int isFilter(char *predicate)
{
	char constant[20];
	sscanf(predicate,"%*u.%*u%*[=<>]%s",constant);

	if(!strstr(constant,"."))
		return 1;
	else
		return 0;
}

int isColEquality(struct PredicateInfo *pInfo)
{return (pInfo->left.relId == pInfo->right.relId); }

unsigned getRelId(struct SelectInfo *sInfo)
{return sInfo->relId;}

unsigned getOriginalRelId(struct QueryInfo *qInfo,struct SelectInfo *sInfo)
{return qInfo->relationIds[sInfo->relId];}

unsigned getColId(struct SelectInfo *sInfo)
{return sInfo->colId;}

uint64_t getConstant(struct FilterInfo *fInfo)
{return fInfo->constant;}

Comparison getComparison(struct FilterInfo *fInfo)
{return fInfo->comparison;}

unsigned getNumOfFilters(struct QueryInfo *qInfo)
{return qInfo->numOfFilters;}

unsigned getNumOfRelations(struct QueryInfo *qInfo)
{return qInfo->numOfRelationIds;}

unsigned getNumOfColEqualities(struct QueryInfo *qInfo)
{
	unsigned sum=0;
	for(unsigned i=0;i<qInfo->numOfPredicates;++i)
		if(isColEquality(&qInfo->predicates[i]))
			++sum;
	return sum;
}

unsigned getNumOfJoins(struct QueryInfo *qInfo)
{
	unsigned sum=0;
	for(unsigned i=0;i<qInfo->numOfPredicates;++i)
		if(!isColEquality(&qInfo->predicates[i]))
			++sum;
	return sum;
}

/**************************** For Testing... ***************************************/
void printTest(struct QueryInfo *qInfo)
{
	for(unsigned j=0;j<qInfo->numOfRelationIds;++j)
	{
		fprintf(stderr,"%u ",qInfo->relationIds[j]);
	}
	fprintf(stderr,"|");
	for(unsigned j=0;j<qInfo->numOfPredicates;++j)
	{
		unsigned leftRelId  = getRelId(&qInfo->predicates[j].left);
		unsigned rightRelId = getRelId(&qInfo->predicates[j].right);
		unsigned leftColId  = getColId(&qInfo->predicates[j].left);
		unsigned rightColId = getColId(&qInfo->predicates[j].right);

		if(isColEquality(&qInfo->predicates[j]))
			fprintf(stderr,"[%u.%u=%u.%u] & ",leftRelId,leftColId,rightRelId,rightColId);
		else
			fprintf(stderr,"%u.%u=%u.%u & ",leftRelId,leftColId,rightRelId,rightColId);
	}
	for(unsigned j=0;j<qInfo->numOfFilters;++j)
	{
		unsigned relId    = getRelId(&qInfo->filters[j].filterLhs);
		unsigned colId    = getColId(&qInfo->filters[j].filterLhs);
		Comparison cmp    = getComparison(&qInfo->filters[j]);
		uint64_t constant = getConstant(&qInfo->filters[j]);

		fprintf(stderr,"%u.%u%c%ld & ",relId,colId,cmp,constant);
	}
	fprintf(stderr,"|");
	for(unsigned j=0;j<qInfo->numOfSelections;++j)
	{
		unsigned relId = getRelId(&qInfo->selections[j]);
		unsigned colId = getColId(&qInfo->selections[j]);
		fprintf(stderr,"%u.%u ",relId,colId);
	}
	fprintf(stderr, "\n");
}
