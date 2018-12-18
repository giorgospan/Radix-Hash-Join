#include <stdio.h>

#include "CUnit/Basic.h"
#include "TestHeader.h"

int init_suite(void)
{return 0;}

int cleanup_suite(void)
{return 0;}

int main(int argc, char const *argv[])
{
 	/* Initialize the CUnit test registry */
	if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();

  	 /* Create 4 Suites */
	CU_pSuite suites[4];
	for(unsigned i=0;i<4;++i){

		char suitName[100];
		sprintf(suitName,"Suite%u",i);
		suites[i] = CU_add_suite(suitName,init_suite,cleanup_suite);
		if (NULL == suites[i]) {
			CU_cleanup_registry();
			return CU_get_error();
		}
  	}

  	/* Add tests to each suite */
	if ((NULL == CU_add_test(suites[0],"Test of Realtion.c::createRelation()",testCreateRelation))||
		(NULL == CU_add_test(suites[0],"Test of Joiner.c::addRelation()",testAddRelation)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suites[1],"Test of Parser.c::createQueryInfo()",testCreateQueryInfo))||
		(NULL == CU_add_test(suites[1],"Test of Parser.c::addFilter()",testAddFilter))||
		(NULL == CU_add_test(suites[1],"Test of Parser.c::getOriginalRelid()",testGetOriginalRelid)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}

	if ((NULL == CU_add_test(suites[2],"Test of Operations.c::colEquality()",testColEquality))||
		(NULL == CU_add_test(suites[2],"Test of Operations.c::colEqualityInter()",testColEqualityInter))||
		(NULL == CU_add_test(suites[2],"Test of Operations.c::filter()",testFilter))||
		(NULL == CU_add_test(suites[2],"Test of Operations.c::fitlerInter()",testFitlerInter)))
	{
		CU_cleanup_registry();
		return CU_get_error();
	}
	if ((NULL == CU_add_test(suites[3],"Test of Partition.c::partition()",testPartition))||
		(NULL == CU_add_test(suites[3],"Test of Partition.c::sortColumn()",testSortColumn))||
		(NULL == CU_add_test(suites[3],"Test of build()|probe()",testBuildProbe)))
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