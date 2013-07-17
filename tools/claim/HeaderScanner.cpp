#include "HeaderScanner.h"

namespace fclaim
{

String HeaderScanner::trimHeader(String text, const char *space)
{
	Ref<StringList> lines = text->split('\n');
	for (int i = 0; i < lines->length(); ++i)
		lines->at(i) = lines->at(i)->trim(space);
	while (lines->length() > 0) {
		if (lines->at(0)->length() == 0)
			lines->pop(0);
		else
			break;
	}
	while (lines->length() > 0) {
		if (lines->at(lines->length() - 1)->length() == 0)
			lines->pop(lines->length() - 1);
		else
			break;
	}
	return lines->join("\n");
}

} // namespace fclaim
