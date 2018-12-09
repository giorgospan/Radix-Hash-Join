/**
 * Unit testing on Radix Hash Join functions [Partition.c | Build.c | Probe.c]
 * 
 * Warning: These tests are based on the fact that HASH_RANGE_1 == 16.
 * 			If we modify it, we need to make some changes in the test functions.
 */
#include <stdio.h>
#include "CUnit/Basic.h"
#include "Operations.h"
#include "Partition.h"
#include "Build.h"
#include "Probe.h"


/**
 * @brief      Tests if histogram and prefix sum are correct.
 */
void testPartition(void)
{
	RadixHashJoinInfo *info;
	JoinArg arg;
	uint64_t col[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u}; 
	arg.isInInter   = 0;
	arg.col         = col;
	arg.tupleSize   = 1;
	arg.numOfTuples = 16;
	info            = partition(&arg);
	
	/* 
	 * HASH_FUN_1(col[i]) will be different for each col element. 
	 * Thus, we expect one column element per bucket. 
	 * HASH_RANGE_1 = 16, check Partition.h
	 * 
	 * pSum[i] will be equal to i. [1 element per bucket]
	 */	
	for(unsigned i=0;i<HASH_RANGE_1;++i)
	{
		CU_ASSERT_EQUAL(1,info->hist[i]);
		CU_ASSERT_EQUAL(i,info->pSum[i]);
	}
	destroyRadixHashJoinInfo(info);
}

/**
 * @brief      Tests if column is sorted appropriately
 */
void testSortColumn()
{
	RadixHashJoinInfo *info;
	JoinArg arg;
	uint64_t col[]  = {15u,14u,13u,12u,11u,10u,9u,8u,7u,6u,5u,
						4u,3u,2u,1u,0u}; 
	arg.isInInter   = 0;
	arg.col         = col;
	arg.tupleSize   = 1;
	arg.numOfTuples = 16;
	info            = partition(&arg);
	for(unsigned i=0;i<HASH_RANGE_1;++i)
		CU_ASSERT_EQUAL(i,info->sorted->values[i]);
	destroyRadixHashJoinInfo(info);
}

void testBuildProbe(void)
{
	RadixHashJoinInfo *infoLeft,*infoRight;
	JoinArg argLeft,argRight;
	uint64_t col1[]  = {0u,1u,2u,3u,4u,5u,6u,7u,8u,9u,10u,
						11u,12u,13u,14u,15u};
	uint64_t col2[]  = {22u,0u,2u,0u,6u};

	argLeft.isInInter    = 0;
	argLeft.col          = col1;
	argLeft.numOfTuples  = 16;
	argLeft.tupleSize    = 1;
	
	argRight.isInInter   = 0;
	argRight.col         = col2;
	argRight.numOfTuples = 5;
	argRight.tupleSize   = 1;
	infoLeft             = partition(&argLeft);
	infoRight            = partition(&argRight);

	build(infoLeft,infoRight);

	struct Vector *result;
	createVector(&result,argLeft.tupleSize+argRight.tupleSize);
	probe(infoLeft,infoRight,result);

	// If we apply join on those two columns, we get 4 <rowId1,rowId2> tuples
	CU_ASSERT_EQUAL(4,getVectorTuples(result));

	destroyRadixHashJoinInfo(infoLeft);
	destroyRadixHashJoinInfo(infoRight);
	destroyVector(&result);

}

int init_suite(void)
{return 0;}

int cleanup_suite(void)
{return 0;}

int main(int argc, char const *argv[])
{
 	/* Initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

	/**
	 * A suite is just a list of tests
	 * Initialization can be anything, like opening a file 
	 * or allcating an array of ints, for example.
	 */
	CU_pSuite mysuite;
	mysuite = CU_add_suite("Suite1",init_suite,cleanup_suite);
	if (NULL == mysuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}
	/**
	 * Add the tests to the suite.
	 */
	if ((NULL == CU_add_test(mysuite,"Test of Partition.c::partition()",testPartition))||
		(NULL == CU_add_test(mysuite,"Test of Partition.c::sortColumn()",testSortColumn))||
		(NULL == CU_add_test(mysuite,"Test of build()|probe()",testBuildProbe)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	/**
	 * Maximum output of run details.
	 * 
	 * The basic interface is also non-interactive, 
	 * with results output to stdout.
	 */
	CU_basic_set_mode(CU_BRM_VERBOSE);
	CU_basic_run_tests();
	/**
	 * Cleanup CU_TestRegistry.
	 * Return any possible error.  
	 */
	CU_cleanup_registry();
	return CU_get_error();
}