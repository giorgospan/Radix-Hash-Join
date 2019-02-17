/**
 * Unit testing on operator's functions [Operations.c]
 *
 * Reminder: operation functions create a result vectors and pass them back to the
 * caller via the struct Vector** argument. [Destruction is needed to avoid mem leaks]
 * Also,note that each "col" array represents a column from the relation.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "CUnit/Basic.h"
#include "Joiner.h"
#include "JobScheduler.h"
#include "Operations.h"

void testColEquality(void)
{
  RADIX_BITS         = 4;
  HASH_RANGE_1       = 16;
  initSize           = 20;
	uint64_t col1[]    = {1u,2u,3u,4,5u,6u,7u,8u,9u,10u};
	uint64_t col2[]    = {10u,20u,30u,40u,50u,60u,70u,80u,90u,100u};
  unsigned numOfRows = 10;

	struct Vector **results = malloc(HASH_RANGE_1*sizeof(struct Vector*));
  MALLOC_CHECK(results);

	colEquality(col1,col1,numOfRows,results);
	CU_ASSERT_EQUAL(10,getVectorTuples(results[0]));
  for(unsigned i=0;i<HASH_RANGE_1;++i)
    destroyVector(results+i);

	colEquality(col1,col2,numOfRows,results);
	CU_ASSERT_EQUAL(0,getVectorTuples(results[0]));
  for(unsigned i=0;i<HASH_RANGE_1;++i)
    destroyVector(results+i);

  free(results);
}


void testFilterFunc(void)
{
  struct Vector *vector;
  struct filterArg arg;

  // createJobScheduler(&js);
  uint64_t col[] = {1u,2u,3u,4u,5u,6u,7u,8u,9u,10u};
  arg.col        = col;
  arg.start      = 0;
  arg.end        = 10;
  arg.cmp        = '>';
  arg.constant   = 6u;
  arg.vector     = &vector;

  filterFunc(&arg);
	CU_ASSERT_EQUAL(4,getVectorTuples(vector));

  destroyVector(&vector);
}

void testFitlerInter(void)
{
  RADIX_BITS         = 4;
  HASH_RANGE_1       = 16;
  initSize           = 20;
  struct Vector *vector;
  struct filterArg arg;

  // createJobScheduler(&js);
  uint64_t col[] = {1u,2u,3u,4u,5u,6u,7u,8u,9u,10u};
  arg.col        = col;
  arg.start      = 0;
  arg.end        = 10;
  arg.cmp        = '>';
  arg.constant   = 6u;
  arg.vector     = &vector;

  // Apply filter
  filterFunc(&arg);
	CU_ASSERT_EQUAL(4,getVectorTuples(vector));
  // Apply filter on the previous result vector
	uint64_t newCol[]   = {0u,0u,0u,0u,0u,0u,0u,8u,9u,0u};
	filterInter(newCol,'>',5u,&vector);
	CU_ASSERT_EQUAL(2,getVectorTuples(vector));

	destroyVector(&vector);
}
