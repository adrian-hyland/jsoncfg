#include <stdio.h>
#include "test.h"


void TestRun(const tTest *Test[], size_t TestCount)
{
	tTestResult Result;
	size_t TestIndex;
	size_t CaseIndex;
	size_t RunCount;
	size_t PassCount;

	printf("Test,Case,Result\n");

	RunCount = 0;
	PassCount = 0;

	for (TestIndex = 0; TestIndex < TestCount; TestIndex++)
	{
		for (CaseIndex = 0; CaseIndex < Test[TestIndex]->CaseCount; CaseIndex++)
		{
			Result = Test[TestIndex]->Case[CaseIndex].Function();
			if (Test[TestIndex]->Name != NULL)
			{
				printf("\"%s\",", Test[TestIndex]->Name);
			}
			else
			{
				printf("\"Test %zu\",", TestIndex + 1);
			}
			if (Test[TestIndex]->Case[CaseIndex].Name != NULL)
			{
				printf("\"%s\",", Test[TestIndex]->Case[CaseIndex].Name);
			}
			else
			{
				printf("\"Case %zu\",", CaseIndex + 1);
			}
			printf("%s\n", TEST_RESULT_GET_STRING(Result));

			RunCount++;
			if (TEST_RESULT_IS_PASS(Result))
			{
				PassCount++;
			}
		}
	}

	printf("\nTests Run : %zu", RunCount);
	printf("\nPasses    : %zu", PassCount);
	printf("\nFailures  : %zu\n", RunCount - PassCount);
}
