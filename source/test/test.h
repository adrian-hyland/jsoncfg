#ifndef TEST_H
#define TEST_H

#include <stdbool.h>
#include <stdio.h>


#ifndef STRINGIFY
/**
 * @brief Macro for 'stringifying' a macro argument
 * @param arg The argument
 * @return The stringified argument
 */
#define STRINGIFY(arg) #arg
#endif


#ifndef TO_STRING
/**
 * @brief Macro for expanding and converting a macro argument to a string value
 * @param arg The argument
 * @return The argument as a string value
 */
#define TO_STRING(arg) STRINGIFY(arg)
#endif


/**
 * @brief Type used for a test result
 */
typedef const char *tTestResult;


/**
 * @brief Constant value for an initial test result value
 */
#define TEST_RESULT_INITIAL NULL


/**
 * @brief Constant value for a passed test result
 */
#define TEST_RESULT_PASS NULL


#ifdef DEBUG
/**
 * @brief Macro for getting a test result failure
 * @param message The failure message for the test result
 * @return The failure test result
 */
#define TEST_RESULT_FAILURE(message) (__builtin_trap(), message)
#else
/**
 * @brief Macro for getting a test result failure
 * @param message The failure message for the test result
 * @return The failure test result
 */
#define TEST_RESULT_FAILURE(message) message
#endif


/**
 * @brief Macro for checking if a test result is a pass
 * @param result The test result
 * @return A true value is returned if the test result is a pass
 * @return A false value is returned if the test result is not a pass
 */
#define TEST_RESULT_IS_PASS(result) ((result) == NULL)


/**
 * @brief Macro for checking if a test result is a failure
 * @param result The test result
 * @return A true value is returned if the test result is a failure
 * @return A false value is returned if the test result is not a failure
 */
#define TEST_RESULT_IS_FAILURE(result) ((result) != NULL)


/**
 * @brief Macro for getting the test result as a string
 * @param result The test result
 * @return The result string
 */
#define TEST_RESULT_GET_STRING(result) (((result) != NULL) ? result : TEST_STRING_PASS)


/**
 * @brief Constant value for a pass test result string
 */
#define TEST_STRING_PASS "PASS"


/**
 * @brief Macro that generates a test failure string
 * @param value     The value being tested
 * @param condition The test condition for the value that failed
 * @return The test failure string
 */
#define TEST_STRING_FAILURE(value, condition) "FAIL: Expected \"" value "\" to be " condition " - see \"" __FILE__ ":" TO_STRING(__LINE__) "\""


/**
 * @brief Type used for a test case function
 * @return The test case function should return \a TEST_RESULT_PASS value if the test case passed.
 * @return The test case function should return \a TEST_RESULT_FAILURE(..) value if the test case failed.
 */
typedef tTestResult (* tTestFunction)(void);


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


/**
 * @brief Macro that tests for a value to be true
 * @param value  The value being tested
 * @param result The current test result
 * @return None
 * @note If \a `value` is not \a `true` then \a `result` will be set to \a `TEST_RESULT_FAILURE(..)`.
 * @note \a `value` will not be tested if the \a `result` has already been set to \a `TEST_RESULT_FAILURE(..)`.
 */
#define TEST_IS_TRUE(value, result)                                      \
	if (TEST_RESULT_IS_PASS(result) && !(value))                          \
	{                                                                     \
		result = TEST_RESULT_FAILURE(TEST_STRING_FAILURE(#value, "true")); \
	}


/**
 * @brief Macro that tests for a value to be false
 * @param value  The value being tested
 * @param result The current test result
 * @return None
 * @note If \a `value` is not \a `false` then \a `result` will be set to \a `TEST_RESULT_FAILURE(..)`.
 * @note \a `value` will not be tested if the \a `result` has already been set to \a `TEST_RESULT_FAILURE(..)`.
 */
#define TEST_IS_FALSE(value, result)                                      \
	if (TEST_RESULT_IS_PASS(result) && (value))                            \
	{                                                                      \
		result = TEST_RESULT_FAILURE(TEST_STRING_FAILURE(#value, "false")); \
	}


/**
 * @brief Macro that tests for a value to be equal to an expected value
 * @param value    The value being tested
 * @param expected The expected value
 * @param result   The current test result
 * @return None
 * @note If \a `value` is not equal to \a `expected` then \a `result` will be set to \a `TEST_RESULT_FAILURE(..)`.
 * @note \a `value` will not be tested if the \a `result` has already been set to \a `TEST_RESULT_FAILURE(..)`.
 */
#define TEST_IS_EQ(value, expected, result)                                                    \
	if (TEST_RESULT_IS_PASS(result) && ((value) != (expected)))                                 \
	{                                                                                           \
		result = TEST_RESULT_FAILURE(TEST_STRING_FAILURE(#value, "equal to \"" #expected "\"")); \
	}


/**
 * @brief Macro that tests for a value to not be equal to an expected value
 * @param value    The value being tested
 * @param expected The expected value
 * @param result   The current test result
 * @return None
 * @note If \a `value` is equal to \a `expected` then \a `result` will be set to \a `TEST_RESULT_FAILURE(..)`.
 * @note \a `value` will not be tested if the \a `result` has already been set to \a `TEST_RESULT_FAILURE(..)`.
 */
#define TEST_IS_NOT_EQ(value, expected, result)                                                    \
	if (TEST_RESULT_IS_PASS(result) && ((value) == (expected)))                                     \
	{                                                                                               \
		result = TEST_RESULT_FAILURE(TEST_STRING_FAILURE(#value, "not equal to \"" #expected "\"")); \
	}


#endif
