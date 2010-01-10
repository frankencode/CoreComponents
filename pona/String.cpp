/*
 * String.cpp -- string processing based on list processing
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "Utf8Source.hpp"
#include "String.hpp"

namespace pona
{

String::String()
	: Ref<Media, Owner>(new Media)
{}

String::String(const Char& ch)
	: Ref<Media, Owner>(new Media)
{
	get()->append(ch);
}

String::String(const char* utf8, int numBytes, int numChars)
	: Ref<Media, Owner>(new Media)
{
	if (numBytes == -1) {
		numBytes = 0;
		if (utf8)
			while (*(utf8 + numBytes)) ++numBytes;
	}
	
	if (numBytes > 0)
	{
		if (numChars == -1) {
			numChars = 0;
			Utf8Source source((uint8_t*)utf8, numBytes);
			while (int(source.numBytesRead()) < numBytes) {
				source.readChar();
				++numChars;
			}
		}
		
		get()->push(0, numChars);
		
		{
			Utf8Source source((uint8_t*)utf8, numBytes);
			for (int i = 0; i < numChars; ++i)
				get()->set(i, source.readChar());
		}
	}
}

String operator*(Char ch, int n)
{
	String s;
	if (n > 0) {
		s->push(0, n);
		for (int i = 0; i < n; ++i)
			s->set(i, ch);
	}
	return s;
}

Ref<StringList, Owner> operator/(String text, String sep)
{
	Ref<StringList, Owner> parts = new StringList();
	
	int i = 0, n = text->length(), m = sep->length();
	while (i < n) {
		int i1 = text->find(i, sep);
		parts->append(text->range(i, i1 - i));
		i = i1 + m;
	}
	if (i == n)
		parts->append(String());
	
	return parts;
}

String operator*(Ref<StringList> parts, String sep)
{
	String text;
	for (int i = 0, n = parts->length(); i < n; ++i) {
		text << parts->get(i);
		if (i != n - 1)
			text << sep;
	}
	return text;
}

} // namespace pona
