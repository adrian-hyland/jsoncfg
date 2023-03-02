#ifdef DEBUG
#include <mcheck.h>
#endif
#include "test_json.h"


static const tTest *Tests[] =
{
	&TestJsonCharacter,
	&TestJsonUtf8,
	&TestJsonUtf16,
	&TestJsonString,
	&TestJsonPath,
	&TestJsonElement,
	&TestJsonParse,
	&TestJsonFormat,
	&TestJsonKeyUtf8,
	&TestJsonValueUtf8,
	&TestJsonLiteralUtf8,
	&TestJsonCommentLineUtf8,
	&TestJsonCommentBlockUtf8
};


int main()
{
#ifdef DEBUG
	mtrace();
#endif

	TestRun(Tests, sizeof(Tests) / sizeof(Tests[0]));

	return 0;
}
