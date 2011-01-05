/*
 * String.cpp -- UTF8 strings
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "Utf8Decoder.hpp"
#include "Crc32.hpp"
#include "FormatSyntax.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Format.hpp"
#include "Variant.hpp"
#include "Path.hpp"
#include "Stream.hpp"
#include "String.hpp"

namespace ftl
{

String::String(const Variant& b)
	: Super(b.toInstance<Media>())
{}

String::String(const Format& b)
{
	*this = String::join(b);
}

String::String(const Path& b)
	: Super(b.toString().media())
{}

void String::validate(const char* data, int size)
{
	if (size < 0) size = str::len(data);
	if (size > 0) {
		Utf8Decoder source(data, size);
		while (source.hasNext())
			source.next();
	}
}

void String::assign(Ref<StringList> parts, const char* sep)
{
	int sepSize = str::len(sep);
	if (parts->length() > 0) {
		int size = 0;
		for (StringList::Index i = parts->first(); parts->def(i); ++i)
			size += parts->at(i)->size();
		size += (parts->length() - 1) * sepSize;
		set(new Media(size));
		char* p = media()->data();
		for (StringList::Index i = parts->first(); parts->def(i); ++i) {
			String part = parts->at(i);
			mem::cpy(p, part->data(), part->size());
			p += part->size();
			if (parts->def(i + 1)) {
				mem::cpy(p, sep, sepSize);
				p += sepSize;
			}
		}
		check(p == media()->data() + size);
	}
	else {
		set(new Media);
	}
}

String String::copy() const
{
	String b;
	b.Super::set(new Media(media()->data(), media()->size()));
	return b;
}

String String::copy(const Index& index0, const Index& index1) const
{
	check(index0.data() == media()->data());
	check(index1.data() == media()->data());
	String s;
	s.assign(index0.pos(), index1.pos() - index0.pos());
	return s;
}

bool String::valid() const
{
	try { validate(); }
	catch (StreamIoException&) { return false; }
	return true;
}

String::Index String::find(const Index& index, const char* pattern) const
{
	if (!index.valid() || media()->isEmpty()) return Index();
	check(index.data() == media()->data());
	const char* t = index.pos(); // text pos
	const char* m = pattern; // match pos
	while ((*t) && (*m)) {
		if (*t == *m)
			++m;
		else
			m = pattern;
		++t;
	}
	return (*m) ? Index() : Index(media()->data(), t - (m - pattern));
}

Ref<StringList, Owner> String::split(const char* pattern) const
{
	Ref<StringList, Owner> parts = new StringList;
	Index index0 = first();
	int patternSize = str::len(pattern);
	while (index0.valid()) {
		Index index1 = find(index0, pattern);
		if (!index1.valid()) break;
		parts->append(copy(index0, index1));
		index0 = Index(media()->data(), index1.pos() + patternSize);
	}
	if (index0.valid())
		parts->append(copy(index0, end()));
	else
		parts->append(String());
	return parts;
}

int String::toInt(bool* ok) const
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		 *ok = (value <= uint64_t(intMax)) && (i1 == media()->size());
	}
	else  {
		*ok = false;
	}
	return sign * int(value);
}

double String::toFloat(bool* ok) const
{
	return toFloat64(ok);
}

int64_t String::toInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == media()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

uint64_t String::toUInt64(bool* ok) const
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(*this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == media()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t String::toFloat64(bool* ok) const
{
	float64_t value = 0.;
	int i1 = 0;
	if (formatSyntax()->floatingPointLiteral()->match(*this, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == media()->size());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

String String::toLower() const
{
	String s2(media()->size());
	for (int i = 0, n = media()->size(); i < n; ++i)
		s2->set(i, ftl::toLower(media()->at(i)));
	return s2;
}

String String::toUpper() const
{
	String s2(media()->size());
	for (int i = 0, n = media()->size(); i < n; ++i)
		s2->set(i, ftl::toUpper(media()->at(i)));
	return s2;
}

String String::stripLeadingSpace() const
{
	int i = 0, n = media()->size();
	while (i < n) {
		if (!ftl::isSpace(media()->at(i))) break;
		++i;
	}
	return String(media()->data() + i, media()->size() - i);
}

String String::stripTrailingSpace() const
{
	int n = media()->size();
	while (n > 0) {
		if (!ftl::isSpace(media()->at(n - 1))) break;
		--n;
	}
	return String(media()->data(), n);
}

String String::trimmed() const
{
	int t = 0, l = 0, n = media()->size();
	while (t < n) {
		if (!ftl::isSpace(media()->at(-t-1))) break;
		++t;
	}
	n -= t;
	while (l < n) {
		if (!ftl::isSpace(media()->at(l))) break;
		++l;
	}
	return String(media()->data() + l, n);
}

String String::stripTags() const
{
	StringList parts;
	char* o = media()->data();
	char* p = o;
	while (*p) {
		if (*p == '<') {
			if (o < p) parts << String(o, p-o);
			while ((*p) && (*p != '>')) ++p;
			p += (*p == '>');
			o = p;
		}
		else if (*p == '&') {
			if (o < p) parts << String(o, p-o);
			while ((*p) && (*p != ';')) ++p;
			p += (*p == ';');
			o = p;
		}
		else {
			++p;
		}
	}
	if (o < p) parts << String(o, p-o);
	return parts.join();
}

String String::simplified() const
{
	return normalized(false);
}

String String::normalized(bool nameCase) const
{
	for (int i = 0, n = media()->size(); i < n; ++i) {
		char ch = media()->at(i);
		if ((0 <= ch) && (ch < 32)) ch = 32;
		media()->set(i, ch);
	}
	Ref<StringList, Owner> parts = split(" ");
	for (StringList::Index i = parts->first(); parts->def(i);) {
		String s = parts->at(i);
		if (s.isEmpty()) {
			parts->remove(i);
		}
		else {
			if (nameCase) {
				s = s.toLower();
				s->set(0, ftl::toUpper(s->at(0)));
				parts->set(i, s);
			}
			++i;
		}
	}
	return String::join(parts, " ");
}

} // namespace ftl
