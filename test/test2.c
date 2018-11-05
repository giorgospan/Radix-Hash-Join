#include "usefulHeaders.h"
#include "prepareInput.h"
#include "phaseOne.h"
#include "bitWiseUtil.h"
#include "structDefinitions.h"
#include "CUnit/Basic.h"

/**
 *
 * This suite tests the following functions
 * 
 * 1. struct PlaceHolder* convertToStructs(uint32_t *column, uint32_t columnSize);
 * 2. uint32_t* createHistogram(struct PlaceHolder* data, uint32_t columnSize);
 * 3. uint32_t** createPsum(uint32_t* hist);
 * 
 */


static uint32_t testColumn1[] = {0,1,2,3,4,5,6,7,8,9};
static uint32_t testColumn2[] = {0,0,0,0,0,0,0,0,0,0};
static uint32_t testColumn3[] = {178,178,178,178,178,178,178,178,178,178};

/* Caution!!!! testColumn4 depends on rangeOfValues */
static uint32_t testColumn4[] = {1,1,1,1,1,1,1,1};
static uint32_t size = 10;


void testConvertToStructs(void)
{
	uint32_t i;
	uint32_t flag=0;
	struct PlaceHolder *ret = convertToStructs(testColumn1,size);
	for (i = 0; i < size; i++)
		if( (ret[i].value != testColumn1[i])||
			(ret[i].hashValue != firstHash(testColumn1[i],significantsForHash))||
			(ret[i].rowId != i))
		{
			flag=1;
			break;
		}

	/* Deallocate the space allocated inside convertToStructs() */
	free(ret);

	if(flag==1)
		CU_FAIL("convertToStructs() failed\n");
}


void testCreateHistogram1(void)
{

	uint32_t i;

	/* Create test-dummy argument to pass createHistogram() */	
	struct PlaceHolder *arr = malloc(size * sizeof(struct PlaceHolder));
	for (i = 0; i < size; i++)
		arr[i].hashValue = firstHash(testColumn2[i], significantsForHash);

	/* Call createHistogram() */
	uint32_t *ret = createHistogram(arr,size);

	/* Checking... */
	CU_ASSERT_EQUAL(ret[0],size);
	for (i = 1; i < size; i++)
	{
		CU_ASSERT_EQUAL(ret[i],0);
	}

	/* Deallocate the space allocated inside createHistogram() */
	free(ret);

	free(arr);
}

void testCreateHistogram2(void)
{
	uint32_t i;

	/* Create test-dummy argument to pass createHistogram() */	
	struct PlaceHolder *arr = malloc(size * sizeof(struct PlaceHolder));
	for (i = 0; i < size; i++)
		arr[i].hashValue = firstHash(testColumn3[i], significantsForHash);

	/* Call createHistogram() */
	uint32_t *ret = createHistogram(arr,size);

	/* Checking... */
	/* Last 3 bits [significants used for hash] of 178 are: 010 ~> 2 */
	CU_ASSERT_EQUAL(ret[2],size);
	for (i = 1; i < size; i++)
	{
		if(i!=2)
		{
			CU_ASSERT_EQUAL(ret[i],0);
		}
	}
	free(ret);
	free(arr);
}


void testCreatePsum(void)
{

	uint32_t** ret = createPsum(testColumn4);
	uint32_t i;
	for (i = 0; i < rangeOfValues; i++)
	{
		CU_ASSERT_EQUAL(*ret[i],i);
	}
	for (i = 0; i < rangeOfValues; i++)
	{
		if (ret[i] != NULL)
			free(ret[i]);
	}
	free(ret);
}


int init_suite(void)
{
	return 0;
}



int cleanup_suite(void)
{
	return 0;
}


int main(int argc, char const *argv[])
{
 	/* Initialize the CUnit test registry */
   if (CUE_SUCCESS != CU_initialize_registry())
      return CU_get_error();


	/**
	 * A suite is just a list of tests
	 *
	 * Initialization can be anything, like opening a file 
	 * or allcating an array of ints, for example.
	 * 
	 */
	CU_pSuite mysuite;
	mysuite = CU_add_suite("Suite2",init_suite,cleanup_suite);
	if (NULL == mysuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/**
	 * Add the tests to the suite.
	 */
	if ((NULL == CU_add_test(mysuite,"Test of convertToStructs()",testConvertToStructs))||
		(NULL == CU_add_test(mysuite,"Test1 of createHistogram()",testCreateHistogram1))||
		(NULL == CU_add_test(mysuite,"Test2 of createHistogram()",testCreateHistogram2))||
		(NULL == CU_add_test(mysuite,"Test of createPsum()",testCreatePsum)))
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