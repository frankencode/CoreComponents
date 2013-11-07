/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

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
