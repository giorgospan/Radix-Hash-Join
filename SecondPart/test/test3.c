/**
 * Unit testing on operator's functions [Operations.c]
 * 
 * Reminder: operation functions create a result vector and pass it back to the
 * caller via the struct Vector** argument. [Destruction is needed to avoid mem leaks]
 * Also,note that each "col" array represents a column from the relation. 
 */
#include <stdio.h>
#include "CUnit/Basic.h"
#include "Operations.h"

void testColEquality(void)
{
	uint64_t col1[] = {1u,2u,3u,4,5u,6u,7u,8u,9u,10u}; 
	uint64_t col2[] = {10u,20u,30u,40u,50u,60u,70u,80u,90u,100u}; 

	struct Vector *results;
	unsigned numOfRows=10;
	colEquality(col1,col1,numOfRows,&results);
	CU_ASSERT_EQUAL(10,getVectorTuples(results));
	destroyVector(&results);
	
	colEquality(col1,col2,numOfRows,&results);
	CU_ASSERT_EQUAL(0,getVectorTuples(results));
	destroyVector(&results);
}

void testColEqualityInter(void)
{
	uint64_t col1[] = {10u,20u,30u,40,50u,60u,0u,0u,0u,0u}; 
	uint64_t col2[] = {10u,20u,30u,40u,50u,60u,70u,80u,90u,100u};
	uint64_t col3[] = {1u,2u,3u,4u,50u,60u,70u,80u,90u,100u};
	struct Vector *results;
	unsigned numOfRows=10;
	colEquality(col1,col2,numOfRows,&results);
	CU_ASSERT_EQUAL(6,getVectorTuples(results));

	unsigned tupleOffset1 = 0;
	unsigned tupleOffset2 = 0;
	colEqualityInter(col2,col3,tupleOffset1,tupleOffset2,&results);
	CU_ASSERT_EQUAL(2,getVectorTuples(results));
	destroyVector(&results);
}


void testFilter(void)
{
	uint64_t col[]     = {1u,2u,3u,4u,5u,6u,7u,8u,9u,10u}; 
	unsigned numOfRows = 10;
	struct Vector *results;
	filter(col,'>',6u,numOfRows,&results);
	CU_ASSERT_EQUAL(4,getVectorTuples(results));
	destroyVector(&results);
}

void testFitlerInter(void)
{
	uint64_t col[]     = {1u,2u,3u,4u,5u,6u,7u,8u,9u,10u}; 
	unsigned numOfRows = 10;
	struct Vector *results;
	filter(col,'>',5u,numOfRows,&results);
	CU_ASSERT_EQUAL(5,getVectorTuples(results));

	uint64_t newCol[]   = {0u,0u,0u,0u,0u,0u,0u,8u,9u,0u}; 
	filterInter(newCol,'>',5u,&results);
	CU_ASSERT_EQUAL(2,getVectorTuples(results));

	destroyVector(&results);
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
	if ((NULL == CU_add_test(mysuite,"Test of Operations.c::colEquality()",testColEquality))||
		(NULL == CU_add_test(mysuite,"Test of Operations.c::colEqualityInter()",testColEqualityInter))||
		(NULL == CU_add_test(mysuite,"Test of Operations.c::filter()",testFilter))||
		(NULL == CU_add_test(mysuite,"Test of Operations.c::fitlerInter()",testFitlerInter)))
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