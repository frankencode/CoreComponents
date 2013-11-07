#ifndef FLUXCLAIM_HEADERSCANNER_H
#define FLUXCLAIM_HEADERSCANNER_H

#include <flux/List.h>
#include "Header.h"

namespace fclaim
{

using namespace flux;

class HeaderScanner: public Object
{
public:
	virtual Ref<Header> scanHeader(String path) const = 0;

protected:
	static String trimHeader(String text, const char *space);
};

typedef List< Ref<HeaderScanner> > HeaderScannerList;

} // namespace fclaim

#endif // FLUXCLAIM_HEADERSCANNER_H
