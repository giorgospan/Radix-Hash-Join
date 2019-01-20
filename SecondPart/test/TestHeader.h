#ifndef TEST_HEADER_H
#define TEST_HEADER_H

/* Testing for Joiner & Relation functions */
void testCreateRelation(void);
void testAddRelation(void);

/* Testing for Queue functions */
void testCreateQueue(void);
void testEnqueue(void);
void testDeQueue(void);

/* Testing Optimizer functions */
void testFindStats(void);

/* Testing for Parser functions */
void testCreateQueryInfo(void);
void testAddFilter(void);
void testAddPredicate(void);
void testGetOriginalRelid(void);

/* Testing for Operators' functions */
void testColEquality(void);
void testFilterFunc(void);
void testFitlerInter(void);

/* Testing for RadixHashJoin functions */
void testPartition(void);
void testPartitionFunc(void);
void testBuildProbe(void);

#endif
