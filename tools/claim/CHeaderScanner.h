#ifndef FCLAIM_CHEADERSCANNER_H
#define FCLAIM_CHEADERSCANNER_H

#include "HeaderScanner.h"

namespace fclaim
{

using namespace fkit;

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

#endif // FCLAIM_CHEADERSCANNER_H
