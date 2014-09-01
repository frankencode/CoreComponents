/*
 * Copyright (C) 2013-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Registry.h"
#include "HeaderStyle.h"

namespace fluxclaim {

HeaderStyle::HeaderStyle(String language)
	: language_(language)
{
	registry()->registerHeaderStyle(this);
}

String HeaderStyle::trimHeader(String text, const char *space)
{
	Ref<StringList> lines = text->split('\n');
	for (int i = 0; i < lines->count(); ++i)
		lines->at(i) = lines->at(i)->trim(space);
	while (lines->count() > 0) {
		if (lines->at(0)->count() == 0)
			lines->pop(0);
		else
			break;
	}
	while (lines->count() > 0) {
		if (lines->at(lines->count() - 1)->count() == 0)
			lines->pop(lines->count() - 1);
		else
			break;
	}
	return lines->join("\n");
}

} // namespace fluxclaim
