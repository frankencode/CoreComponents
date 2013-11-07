#ifndef FLUXCLAIM_CHEADERSCANNER_H
#define FLUXCLAIM_CHEADERSCANNER_H

#include "HeaderScanner.h"

namespace fclaim
{

using namespace flux;

class CHeaderScannerInitializer
{
public:
	CHeaderScannerInitializer();
private:
	static int count_;
};

namespace { CHeaderScannerInitializer initializer; }

class CHeaderSyntax;

class CHeaderScanner: public HeaderScanner
{
public:
	Ref<Header> scanHeader(String path) const;

private:
	friend class CHeaderScannerInitializer;
	CHeaderScanner();
	Ref<CHeaderSyntax> headerSyntax_;
};

} // namespace fclaim

#endif // FLUXCLAIM_CHEADERSCANNER_H
