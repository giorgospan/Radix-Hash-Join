/**
 * Unit testing on Optimizer functions [Optimizer.c]
 */
#include <stdio.h>
#include <stdlib.h>
#include "CUnit/Basic.h"
#include "Optimizer.h"


void testFindStats(void)
{

  struct columnStats stat;
  unsigned colSize = 16;
  uint64_t col[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};

  findStats(col,&stat,colSize);
  CU_ASSERT_EQUAL(stat.maxValue,15u);
  CU_ASSERT_EQUAL(stat.minValue,0u);
  CU_ASSERT_EQUAL(stat.f,16);
  CU_ASSERT_EQUAL(stat.discreteValues,16);

  free(stat.bitVector);
}
