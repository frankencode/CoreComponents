/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "HeaderScanner.h"

namespace fclaim
{

String HeaderScanner::trimHeader(String text, const char *space)
{
	Ref<StringList> lines = text->split('\n');
	for (int i = 0; i < lines->size(); ++i)
		lines->at(i) = lines->at(i)->trim(space);
	while (lines->size() > 0) {
		if (lines->at(0)->size() == 0)
			lines->pop(0);
		else
			break;
	}
	while (lines->size() > 0) {
		if (lines->at(lines->size() - 1)->size() == 0)
			lines->pop(lines->size() - 1);
		else
			break;
	}
	return lines->join("\n");
}

} // namespace fclaim
