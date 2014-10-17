/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#include <flux/ByteArray>
#include <flux/Unicode>

namespace flux {

Ref<ByteArray> Unicode::copy(int i0, int i1) const
{
	FLUX_ASSERT(i0 <= i1);
	walkTo(i0);
	const char *p0 = walker_.pos();
	while (i_ < i1) { ++walker_; ++i_; }
	return ByteArray::copy(p0, walker_.pos() - p0);
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

Ref<StringList> String::split(const char *pattern) const
{
	Ref<StringList> parts = StringList::create();
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

} // namespace flux
