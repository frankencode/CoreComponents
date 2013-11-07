/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

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
