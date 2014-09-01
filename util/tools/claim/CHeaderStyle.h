/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_CHEADERSTYLE_H
#define FLUXCLAIM_CHEADERSTYLE_H

#include "HeaderStyle.h"

namespace fluxclaim {

using namespace flux;

class CHeaderStyle: public HeaderStyle
{
public:
	virtual Ref<Header> scan(String path) const;
	virtual String str(Notice *notice) const;

private:
	friend class Registration<CHeaderStyle>;

	CHeaderStyle();
	Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<CHeaderStyle> registration; }

} // namespace fluxclaim

#endif // FLUXCLAIM_CHEADERSTYLE_H
