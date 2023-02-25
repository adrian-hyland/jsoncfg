#ifdef DEBUG
#include <mcheck.h>
#endif
#include "test_json.h"


static const tTest *Tests[] =
{
	&TestJsonCharacter,
	&TestJsonUtf8,
	&TestJsonString,
	&TestJsonPath,
	&TestJsonElement,
	&TestJsonParse,
	&TestJsonFormat,
	&TestJsonValueUtf8
};


int main()
{
#ifdef DEBUG
	mtrace();
#endif

	TestRun(Tests, sizeof(Tests) / sizeof(Tests[0]));

	return 0;
}
