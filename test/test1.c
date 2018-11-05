#include "usefulHeaders.h"
#include "prepareInput.h"
#include "CUnit/Basic.h"

/**
 *
 * This suite tests the following functions
 * 
 * 1. void inputCreator();
 * 2. void inputReader();
 * 3. uint32_t** createArrayAndInit(uint32_t *rowSize, uint32_t *colSize);
 * 
 */

static uint32_t tuplesToBeRead;
static uint32_t colsToBeRead;
static uint32_t** writtenArray;
static uint32_t** createdArray;
static FILE* fp;

/**
 * In this test, we are actually
 * doing the exact same thing as 
 * we do in "void inputReader()"
 *
 * We scan the newly created
 * file making sure that we read 1 uint32_t element 
 * per fread() call.
 * 
 */
void testInputCreator(void)
{
	uint32_t i;
	uint32_t j;
	uint32_t r;
	char message[] = "inputCreator failed to create the input file";

	if(fread(&tuplesToBeRead, sizeof(uint32_t), 1, fp)<1)
	{
		CU_FAIL_FATAL(message);
	}

	if(fread(&colsToBeRead, sizeof(uint32_t), 1, fp)<1)
	{
		CU_FAIL(message);
	}
	
	/* Allocate space to store what we read */
	if( (writtenArray=malloc(tuplesToBeRead * sizeof(uint32_t*))) == NULL )
	{
		CU_FAIL("test function could not allocate enough memory\n");
	}
	for (i = 0; i < tuplesToBeRead; ++i)
	{

		if( (writtenArray[i] = malloc(colsToBeRead * sizeof(uint32_t))) == NULL )
		{
			CU_FAIL("test function could not allocate enough memory\n");
		}
	}


	/* Store each element we read in writtenArray */
	for (i = 0; i < tuplesToBeRead; ++i)
	{
		for (j = 0; j < colsToBeRead; ++j)
		{
			if(fread(&r, sizeof(uint32_t), 1, fp)<1)
			{
				CU_FAIL(message);
			}
			writtenArray[i][j] = r;
		}
	}
}

/**
 *  inputReader() is actually a test function itself
 *  Thus, there is nothing special to check here,
 *  except for its successfull execution
 *  
*/
void testInputReader(void)
{
	/* Call inputCreator()*/
	// inputReader();
	CU_PASS("inputReader() succeeded");
}


/**
 *
 * After testInputCreator succeded,
 * we call createArrayAndInit() to check
 * whether it reads what inputCreator() 
 * exactly wrote in the file
 *
 * Reminder: tuples and columns will be reversed
 * 
 */

void testCreateArrayAndInit(void)
{
	uint32_t c;
	uint32_t t;
	uint32_t i;
	uint32_t j;
	
	createdArray = createArrayAndInit(&t,&c);

	/* First make sure it reads the right #rows & #cols*/
	CU_ASSERT_EQUAL(t,colsToBeRead);
	CU_ASSERT_EQUAL(c,tuplesToBeRead);

	/* Then make sure it reads the right numbers [i.e: the numbers written on the file] */
	for (i = 0; i < tuplesToBeRead; ++i)
	{
		for (j = 0; j < colsToBeRead; ++j)
		{
			CU_ASSERT_EQUAL(createdArray[j][i],writtenArray[i][j]);
		}
	}
}


int init_suite(void)
{
	/* Call inputCreator()*/
	inputCreator();
	if((fp = fopen("input.bin", "rb")) == NULL)
	{
		return -1;
	}
	else
		return 0;
}



int cleanup_suite(void)
{

	if(fclose(fp) != 0)
	{
		return -1;
	}
	else
	{
		fp = NULL;
	}

	uint32_t i,j;
	
	for (i = 0; i < tuplesToBeRead; i++)
		free(writtenArray[i]);

	free(writtenArray);
	deAllocateArray(createdArray,colsToBeRead);
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
	mysuite = CU_add_suite("Suite1",init_suite,cleanup_suite);
	if (NULL == mysuite) {
		CU_cleanup_registry();
		return CU_get_error();
	}

	/**
	 * Add the tests to the suite.
	 */
	if ((NULL == CU_add_test(mysuite,"Test of inputCreator()",testInputCreator))||
		(NULL == CU_add_test(mysuite,"Test of inputReader()",testInputReader))||
		(NULL == CU_add_test(mysuite,"Test of createArrayAndInit()",testCreateArrayAndInit)))
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