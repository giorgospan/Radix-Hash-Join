#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "Utils.h"

struct SelectInfo
{
	unsigned relId;
	unsigned colId;
};

struct FilterInfo
{
	struct SelectInfo filterLhs;
	enum Comparison comparison;
	uint64_t constant;
};

struct PredicateInfo
{
	struct SelectInfo left;
	struct SelectInfo right;
};

struct QueryInfo
{
	unsigned *relationIds;
	struct PredicateInfo *predicates;
	struct FilterInfo *filters;
	struct SelectInfo *selections;
	unsigned numOfRelationIds;
	unsigned numOfPredicates;
	unsigned numOfFilters;
	unsigned numOfSelections;
};

/**
 * @brief      Creates a new query structure.
 *             Subsequently, parseQuery(..) is called.
 */
void createQueryInfo(struct QueryInfo **qInfo,char *rawQuery);

/**
 * @brief      Deallocates any space allocated for qInfo members
 */
void destroyQueryInfo(struct QueryInfo *qInfo);

/**
 * @brief      Parses relation ids <r1> <r2> ...
 */
void parseRelationIds(struct QueryInfo *qInfo,char *rawRelations);

/**
 * @brief      Parses predicates r1.a=r2.b&r1.b=r3.c...
 */
void parsePredicates(struct QueryInfo *qInfo,char *rawPredicates);

/**
 * @brief      Parses selections r1.a r1.b r3.c...
 */
void parseSelections(struct QueryInfo *qInfo,char *rawSelections);

/**
 * @brief      Parses selections [RELATIONS]|[PREDICATES]|[SELECTS]
 */
void parseQuery(struct QueryInfo *qInfo,char *rawQuery);

/**
 * @brief      Determines if predicate is filter
 *
 * @param      predicate  The predicate 
 *
 * @return     True if filter, False otherwise.
 */
int isFilter(char *predicate);

int isColEquality(struct PredicateInfo *pInfo);
void addFilter(struct FilterInfo *fInfo,char *token);
void addPredicate(struct PredicateInfo *pInfo,char *token);

/* 
 * "Getter" functions. 
 * Despite having access to each struct's members from anywhere in our program,
 * we use "getter" functions just to make our code more neat & clean.
 */
unsigned getOriginalRelId(struct QueryInfo *qInfo,struct SelectInfo *sInfo);
unsigned getRelId(struct SelectInfo *sInfo);
unsigned getColId(struct SelectInfo *sInfo);
uint64_t getConstant(struct FilterInfo *fInfo);
Comparison getComparison(struct FilterInfo *fInfo);
unsigned getNumOfRelations(struct QueryInfo *qInfo);
unsigned getNumOfFilters(struct QueryInfo *qInfo);
unsigned getNumOfColEqualities(struct QueryInfo *qInfo);
unsigned getNumOfJoins(struct QueryInfo *qInfo);

void printTest(struct QueryInfo *qInfo);
#endif