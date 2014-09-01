/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_SHHEADERSTYLE_H
#define FLUXCLAIM_SHHEADERSTYLE_H

#include "HeaderStyle.h"

namespace fluxclaim {

using namespace flux;

class ShHeaderStyle: public HeaderStyle
{
public:
	virtual Ref<Header> scan(String path) const;
	virtual String str(Notice *notice) const;

private:
	friend class Registration<ShHeaderStyle>;

	ShHeaderStyle();
	Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<ShHeaderStyle> registration; }

} // namespace fluxclaim

#endif // FLUXCLAIM_SHHEADERSTYLE_H
