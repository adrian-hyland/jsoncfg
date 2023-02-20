#ifdef DEBUG
#include <mcheck.h>
#endif
#include "test_json.h"


static const tTest *Tests[] =
{
	&TestJsonCharacter,
	&TestJsonString,
	&TestJsonPath,
	&TestJsonElement,
	&TestJsonParse,
	&TestJsonFormat
};


int main()
{
#ifdef DEBUG
	mtrace();
#endif

	TestRun(Tests, sizeof(Tests) / sizeof(Tests[0]));

	return 0;
}
