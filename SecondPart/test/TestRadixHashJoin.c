/**
 * Unit testing on Radix Hash Join functions [Partition.c | Build.c | Probe.c]
 * 
 * Warning: These tests are based on the fact that HASH_RANGE_1 == 16.
 * 			If we modify it, we need to make some changes in the test functions.
 */
#include <stdio.h>
#include "CUnit/Basic.h"
#include "Intermediate.h"
#include "Operations.h"
#include "Partition.h"
#include "Build.h"
#include "Probe.h"


/**
 * @brief      Tests if histogram and prefix sum are correct.
 */
void testPartition(void)
{
	RadixHashJoinInfo info;
	uint64_t col[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u}; 
	info.isInInter   = 0;
	info.col         = col;
	info.tupleSize   = 1;
	info.numOfTuples = 16;
	partition(&info);
	
	/* 
	 * HASH_FUN_1(col[i]) will be different for each col element. 
	 * Thus, we expect one column element per bucket. 
	 * HASH_RANGE_1 = 16, check Partition.h
	 * 
	 * pSum[i] will be equal to i. [1 element per bucket]
	 */	
	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		CU_ASSERT_EQUAL(1,info.hist[i]);
		CU_ASSERT_EQUAL(i,info.pSum[i]);
	}
}

/**
 * @brief      Tests if column is sorted appropriately
 */
void testSortColumn(void)
{
	RadixHashJoinInfo info;
	uint64_t col[]  = {15u,14u,13u,12u,11u,10u,9u,8u,7u,6u,5u,
						4u,3u,2u,1u,0u}; 
	info.isInInter   = 0;
	info.col         = col;
	info.tupleSize   = 1;
	info.numOfTuples = 16;
	partition(&info);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		CU_ASSERT_EQUAL(i,info.sorted->values[i]);
}

void testBuildProbe(void)
{
	RadixHashJoinInfo infoLeft,infoRight;
	uint64_t col1[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};
	uint64_t col2[]  = {22u,0u,2u,0u,6u};

	infoLeft.isInInter    = 0;
	infoLeft.col          = col1;
	infoLeft.numOfTuples  = 16;
	infoLeft.tupleSize    = 1;
	
	infoRight.isInInter   = 0;
	infoRight.col         = col2;
	infoRight.numOfTuples = 5;
	infoRight.tupleSize   = 1;
	partition(&infoLeft);
	partition(&infoRight);

	build(&infoLeft,&infoRight);

	struct Vector *result;
	createVector(&result,infoLeft.tupleSize+infoRight.tupleSize);
	probe(&infoLeft,&infoRight,result);

	// If we apply join on those two columns, we get 4 <rowId1,rowId2> tuples
	CU_ASSERT_EQUAL(4,getVectorTuples(result));

	destroyVector(&result);
}
