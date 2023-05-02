#if defined(DEBUG) && !defined(__WIN32__)
#include <mcheck.h>
#endif
#include "test_json.h"


static const tTest *Tests[] =
{
	&TestJsonCharacter,
	&TestJsonUtf8,
	&TestJsonUtf16,
	&TestJsonUtf,
	&TestJsonString,
	&TestJsonPath,
	&TestJsonElement,
	&TestJsonParse,
	&TestJsonFormat,
	&TestJsonMain,
};


int main()
{
#if defined(DEBUG) && !defined(__WIN32__)
	mtrace();
#endif

	TestRun(Tests, sizeof(Tests) / sizeof(Tests[0]));

	return 0;
}
