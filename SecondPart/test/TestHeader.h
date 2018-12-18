#ifndef TEST_HEADER_H
#define TEST_HEADER_H

void testCreateRelation(void);
void testAddRelation(void);

void testCreateQueryInfo(void);
void testAddFilter(void);
void testAddPredicate(void);
void testGetOriginalRelid(void);

void testColEquality(void);
void testColEqualityInter(void);
void testFilter(void);
void testFitlerInter(void);

void testPartition(void);
void testSortColumn(void);
void testBuildProbe(void);

#endif