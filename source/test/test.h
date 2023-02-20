#ifndef TEST_H
#define TEST_H

#include <stddef.h>
#include <stdbool.h>


/**
 * @brief Type used for a test case function
 * @return The test case function should return a true value if the test case passed 
 * @return The test case function should return a false value if the test case failed 
 */
typedef bool (* tTestFunction)(void);


/**
 * @brief Type used for a test case
 */
typedef struct
{
	const char         *Name;     /**< The name of the test case */
	const tTestFunction Function; /**< The test case function */
} tTestCase;


/**
 * @brief Type used for a test
 */
typedef struct
{
	const char       *Name;      /**< The name of the test */
	const tTestCase  *Case;      /**< Array of test cases */
	size_t            CaseCount; /**< The number of test cases */
} tTest;


/**
 * @brief Runs a set of tests
 * @param Test      The tests to run
 * @param TestCount The number of tests to run
 * @return None
 */
void TestRun(const tTest *Test[], size_t TestCount);


#endif
