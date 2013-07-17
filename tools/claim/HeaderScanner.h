#ifndef FCLAIM_HEADERSCANNER_H
#define FCLAIM_HEADERSCANNER_H

#include <fkit/List.h>
#include "Header.h"

namespace fclaim
{

using namespace fkit;

class HeaderScanner: public Object
{
public:
	virtual Ref<Header> scanHeader(String path) const = 0;

protected:
	static String trimHeader(String text, const char *space);
};

typedef List< Ref<HeaderScanner> > HeaderScannerList;

} // namespace fclaim

#endif // FCLAIM_HEADERSCANNER_H
