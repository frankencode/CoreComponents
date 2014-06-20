/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_SHHEADERSCANNER_H
#define FLUXCLAIM_SHHEADERSCANNER_H

#include "HeaderScanner.h"

namespace fluxclaim
{

using namespace flux;

class ShHeaderScannerInitializer
{
public:
	ShHeaderScannerInitializer();
private:
	static int count_;
};

namespace { ShHeaderScannerInitializer initializer; }

class ShHeaderSyntax;

class ShHeaderScanner: public HeaderScanner
{
public:
	Ref<Header> scanHeader(String path) const;

private:
	friend class ShHeaderScannerInitializer;
	ShHeaderScanner();
	Ref<ShHeaderSyntax> headerSyntax_;
};

} // namespace fluxclaim

#endif // FLUXCLAIM_SHHEADERSCANNER_H
