/**
 * Unit testing on joiner and iation functions [Joiner.c | Relation.c]
 */
#include <stdio.h>
#include <time.h>/*time()*/
#include <stdlib.h>/*rand()*/
#include "CUnit/Basic.h"
#include "Utils.h"
#include "Relation.h"
#include "Joiner.h"

void testCreateRelation(void)
{	
	char sameFile[] = "r0";
	struct Relation *R1,*R2;

	// NOTE: Remember to change the file path in Relation.h !!!
	createRelation(&R1,sameFile);
	createRelation(&R2,sameFile);
	
	CU_ASSERT_EQUAL(R1->numOfTuples,R2->numOfTuples);
	CU_ASSERT_EQUAL(R1->numOfCols,R2->numOfCols);

	for(unsigned c=0;c<R1->numOfCols;++c)
		for(unsigned r=0;r<R1->numOfTuples;++r)
			CU_ASSERT_EQUAL(R1->columns[c][r],R2->columns[c][r]);

	destroyRelation(R1);
	destroyRelation(R2);
}

/* Also tests getRelationTuples(..) */
void testAddRelation(void)
{
	struct Joiner *J;
	createJoiner(&J);
	const int NUM_OF_RELS = 13;
	J->numOfRelations     = NUM_OF_RELS;
	J->relations          = allocate(J->numOfRelations*sizeof(struct Relation*),"testAddRelation");
	
	for(unsigned i=0;i<NUM_OF_RELS;++i)
	{
		char name[10];
		sprintf(name,"r%u",i);
		addRelation(J,name);
	}
	srand(time(NULL));
	unsigned relId = rand() % 13;
	char name[100];
	sprintf(name,"./workloads/small/r%u",relId);

	FILE *fp = fopen(name,"rb");
	uint64_t tuples;
	fread(&tuples,sizeof(uint64_t),1,fp);

	CU_ASSERT_EQUAL(getRelationTuples(J,relId),tuples);

	fclose(fp);
	destroyJoiner(J);
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
	if ((NULL == CU_add_test(mysuite,"Test of Realtion.c::createRelation()",testCreateRelation))||
		(NULL == CU_add_test(mysuite,"Test of Joiner.c::addRelation()",testAddRelation)))
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