/*
 * CharList.cpp -- Unicode character list
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "FormatSyntax.hpp"
#include "IntegerLiteral.hpp"
#include "FloatLiteral.hpp"
#include "Utf8Sink.hpp"
#include "Crc32.hpp"
#include "CharList.hpp"

namespace pona
{

int CharList::toInt(bool* ok)
{
	bool h;
	if (!ok) ok = &h;
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(this, 0, &i1, &value, &sign)) {
		 *ok = (value <= uint64_t(intMax)) && (i1 == length());
	}
	else  {
		*ok = false;
	}
	return sign * int(value);
}

double CharList::toFloat(bool* ok)
{
	return toFloat64(ok);
}

int64_t CharList::toInt64(bool* ok)
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = ((value & (uint64_t(1) << 63)) != 0) && (i1 == length());
	}
	else {
		if (ok)
			*ok = false;
	}
	return sign * value;
}

uint64_t CharList::toUInt64(bool* ok)
{
	uint64_t value = 0;
	int sign = 0;
	int i1 = 0;
	if (formatSyntax()->integerLiteral()->match(this, 0, &i1, &value, &sign)) {
		if (ok)
			*ok = (sign == 1) && (i1 == length());
	}
	else {
		if (ok)
			*ok = false;
	}
	return value;
}

float64_t CharList::toFloat64(bool* ok)
{
	float64_t value = 0.;
	int i1 = 0;
	
	if (formatSyntax()->floatingPointLiteral()->match(this, 0, &i1, &value)) {
		if (ok)
			*ok = (i1 == length());
	}
	else {
		if (ok)
			*ok = false;
	}
	
	return value;
}

Ref<CharList, Owner> CharList::toLower() const
{
	Ref<CharList, Owner> s2 = new CharList;
	s2->push(0, length());
	for (int i = 0, n = length(); i < n; ++i)
		s2->set(i, get(i).toLower());
	return s2;
}

Ref<CharList, Owner> CharList::toUpper() const
{
	Ref<CharList, Owner> s2 = new CharList;
	s2->push(0, length());
	for (int i = 0, n = length(); i < n; ++i)
		s2->set(i, get(i).toUpper());
	return s2;
}

Ref<CharList, Owner> CharList::stripLeadingSpace() const
{
	int n = length();
	while (n > 0) {
		Char ch = get(-n);
		bool isSpace = ((ch == ' ') || (ch == '\t'));
		if (!isSpace) break;
		--n;
	}
	return copy(length() - n, n);
}

Ref<CharList, Owner> CharList::stripTrailingSpace() const
{
	int n = length();
	while (n > 0) {
		Char ch = get(n - 1);
		bool isSpace = ((ch == ' ') || (ch == '\t'));
		if (!isSpace) break;
		--n;
	}
	return copy(0, n);
}

CString CharList::utf8() const
{
	int numChars = length();
	int numBytes = 0;
	{
		Utf8Sink nullSink;
		for (int i = 0; i < numChars; ++i)
			nullSink.writeChar(get(i));
		numBytes = nullSink.numBytesWritten();
	}
	
	CString buf(numBytes);
	Utf8Sink sink((uint8_t*)buf->data(), numBytes);
	for (int i = 0; i < numChars; ++i)
		sink.writeChar(get(i));
	return buf;
}

uint32_t CharList::crc32() const
{
	CString cs = utf8();
	Crc32 crc;
	crc.feed(cs, cs->length());
	return crc.sum();
}

char* CharList::strdup() const { return pona::strdup(utf8()->data()); }

CharList::CharList(Ref<Super> parent, int i, int n)
	: Super(parent, i, n)
{}

Ref<CharList::Super, Owner> CharList::newList() const
{
	return new CharList;
}

Ref<CharList::Super, Owner> CharList::newChildList(Ref<Super> parent, int i, int n) const
{
	return new CharList(parent, i, n);
}

} // namespace pona
