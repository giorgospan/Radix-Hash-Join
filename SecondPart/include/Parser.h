#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>

typedef enum Comparison { Less='<', Greater='>', Equal='=' } Comparison;

struct SelectInfo
{
	uint64_t relId;
	uint64_t colId;
};

void createSelectInfo(struct SelectInfo **sInfo,uint64_t relId,uint64_t colId);


struct FilterInfo
{
	struct SelectInfo filterColumn;
	enum Comparison comparison;
	uint64_t constant;
};

void createFilterInfo(struct SelectInfo **fInfo,struct SelectInfo sInfo,Comparison cmp,uint64_t constant);


struct PredicateInfo
{
	struct SelectInfo left;
	struct SelectInfo right;
};

void createPredicateInfo(struct SelectInfo **pInfo,struct SelectInfo left,struct SelectInfo right);

struct QueryInfo
{
	uint64_t *relationIds;
	struct PredicateInfo *predicates;
	struct FilterInfo *filters;
	struct SelectInfo *selections;
};

/**
 * @brief  Creates a new query structure and initialize 
 *         its fields to NULL. Subsequently, parseQuery(..) is called.
 */
void createQueryInfo(struct QueryInfo **qInfo,char *rawQuery);


/**
 * @brief      Parses relation ids <r1> <r2> ...
 */
void parseRelationIds(char *rawRelations);


/**
 * @brief      Parses predicates r1.a=r2.b&r1.b=r3.c...
 */
void parsePredicates(char *rawPredicates);


/**
 * @brief      Parses selections r1.a r1.b r3.c...
 */
void parseSelections(char *rawSelections);


/**
 * @brief      Parses selections [RELATIONS]|[PREDICATES]|[SELECTS]
 */
void parseQuery(char *rawQuery);

#endif