/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
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
	virtual int magicCount(String text) const;

private:
	friend class Registration<ShHeaderStyle>;

	ShHeaderStyle();
	Ref<SyntaxDefinition> headerSyntax_;
};

namespace { Registration<ShHeaderStyle> registration; }

} // namespace fluxclaim

#endif // FLUXCLAIM_SHHEADERSTYLE_H
