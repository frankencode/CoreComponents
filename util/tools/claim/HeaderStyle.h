/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXCLAIM_HEADERSTYLE_H
#define FLUXCLAIM_HEADERSTYLE_H

#include <flux/Registration.h>
#include <flux/List.h>
#include "Header.h"
#include "Notice.h"

namespace fluxclaim {

using namespace flux;

class HeaderStyle: public Object
{
public:
	inline String language() const { return language_; }

	virtual Ref<Header> scan(String path) const = 0;
	virtual String str(Notice *notice) const = 0;

protected:
	static String trimHeader(String text, const char *space);

	HeaderStyle(String language);

	Ref<HeaderStyle> headerStyle_;
	String language_;
};

typedef List< Ref<HeaderStyle> > HeaderStyleList;

} // namespace fluxclaim

#endif // FLUXCLAIM_HEADERSTYLE_H
