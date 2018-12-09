/**
 * Unit testing on parser's functions [Parser.c]
 */
#include <stdio.h>
#include <time.h>/*time()*/
#include <stdlib.h>/*rand()*/
#include "CUnit/Basic.h"
#include "Parser.h"

/**
 * This function not only tests createQueryInfo(..), but also :
 * parseQuery(..), parseRelationIds(..), parseSelections(..) and parsePredicates(..)
 * because these function are called from one another.
 */
void testCreateQueryInfo(void)
{
	// Query with 4 relIds, 3 filters and 1 column equality predicate.
	char rawQuery[] = "12 1 6 12|3.0=3.2&0.2=1.0&1.2>9999&1.1=4444&1.0=2.1&0.1=3.2&3.0<33199|2.1 0.1 0.2";
	struct QueryInfo *q;
	createQueryInfo(&q,rawQuery);
	CU_ASSERT_EQUAL(4,getNumOfRelations(q));
	CU_ASSERT_EQUAL(3,getNumOfFilters(q));
	CU_ASSERT_EQUAL(1,getNumOfColEqualities(q));
	destroyQueryInfo(q);	
}

/**
 * This function acts as a test for getComparison() too.
 */
void testAddFilter(void)
{
	struct FilterInfo f;
	char string1[] = "4.0<3.1";
	addFilter(&f,string1);
	CU_ASSERT_EQUAL('<',getComparison(&f));

	char string2[] = "4.0>3.1";
	addFilter(&f,string2);
	CU_ASSERT_EQUAL('>',getComparison(&f));

	char string3[] = "4.0=3.1";
	addFilter(&f,string3);
	CU_ASSERT_EQUAL('=',getComparison(&f));
}

/**
 * This function acts also as a test for getColId() and getRelId()
 */
void testAddPredicate(void)
{
	struct PredicateInfo p;
	char string[] = "3.1=5.2";
	addPredicate(&p,string);
	CU_ASSERT_EQUAL(getColId(&p.left),3);
	CU_ASSERT_EQUAL(getColId(&p.right),5);
	CU_ASSERT_EQUAL(getRelId(&p.left),1);
	CU_ASSERT_EQUAL(getRelId(&p.right),2);
}


void testGetOriginalRelid(void)
{
	char rawQuery[] = "12 1 6 12|3.0=1.2&0.2=1.0&1.0=2.1&0.1=3.2&3.0<33199|2.1 0.1 0.2";
	struct QueryInfo *q;
	createQueryInfo(&q,rawQuery);
	struct PredicateInfo p = q->predicates[0];
	CU_ASSERT_EQUAL(12,getOriginalRelId(q,&p.left));
	CU_ASSERT_EQUAL(1,getOriginalRelId(q,&p.right));
	destroyQueryInfo(q);
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
	if ((NULL == CU_add_test(mysuite,"Test of Parser.c::createQueryInfo()",testCreateQueryInfo))||
		(NULL == CU_add_test(mysuite,"Test of Parser.c::addFilter()",testAddFilter))||
		(NULL == CU_add_test(mysuite,"Test of Parser.c::getOriginalRelid()",testGetOriginalRelid)))
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