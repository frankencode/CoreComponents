/*
 * Character.cpp -- access UTF-8 encoded characters by index
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "ByteArray.hpp"
#include "Character.hpp"

namespace ftl
{

Ref<ByteArray, Owner> Character::copy(int i0, int i1) const
{
	FTL_ASSERT(i0 <= i1);
	walkTo(i0);
	const char *p0 = walker_.pos();
	while (i_ < i1) { ++walker_; ++i_; }
	return ByteArray::newInstance(p0, walker_.pos() - p0);
}

/*int String::find(int i, const char *pattern) const
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

Ref<StringList, Owner> String::split(const char *pattern) const
{
	Ref<StringList, Owner> parts = StringList::newInstance();
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
		parts->append(String());
	return parts;
}*/

} // namespace ftl
