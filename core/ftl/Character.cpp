 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include "ByteArray.hpp"
#include "Character.hpp"

namespace ftl
{

hook<ByteArray> Character::copy(int i0, int i1) const
{
	FTL_ASSERT(i0 <= i1);
	walkTo(i0);
	const char *p0 = walker_.pos();
	while (i_ < i1) { ++walker_; ++i_; }
	return ByteArray::copy(p0, walker_.pos() - p0);
}

/*int string::find(int i, const char *pattern) const
{
	if (!Super::get()->chars()->has(i)) return i;
	const char *t = Super::get()->chars()->byte(i); // text pos
	const char *m = pattern; // match pos
	while ((*t) && (*m)) {
		if (*t == *m)
			++m;
		else
			m = pattern;
		++t;
	}
	return (*m) ? Super::get()->chars()->length() : Super::get()->chars()->index(t - (m - pattern));
}

hook<StringList> string::split(const char *pattern) const
{
	hook<StringList> parts = StringList::create();
	int i0 = 0;
	int patternLength = Utf8Walker::length(pattern);
	while (i0 < Super::get()->chars()->length()) {
		int i1 = find(i0, pattern);
		if (i1 == Super::get()->chars()->length()) break;
		parts->append(Super::get()->chars()->copy(i0, i1));
		i0 = i1 + patternLength;
	}
	if (i0 < Super::get()->chars()->length())
		parts->append(Super::get()->chars()->copy(i0, Super::get()->chars()->length()));
	else
		parts->append(string());
	return parts;
}*/

} // namespace ftl
