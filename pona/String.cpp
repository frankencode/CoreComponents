/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include "NumberFormatting.hpp"
#include "Utf8Source.hpp"
#include "Utf8Sink.hpp"
#include "String.hpp"

namespace pona
{

String::String()
	: list_(new Media)
{}

String::String(int n)
	: list_(new Media)
{
	if (n > 0)
		list_->push(0, n);
}

String::String(int n, Char ch)
	: list_(new Media)
{
	if (n > 0) {
		list_->push(0, n);
		for (int i = 0; i < n; ++i)
			list_->set(i, ch);
	}
}

String::String(const char* utf8, int numBytes, int numChars)
	: list_(new Media)
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
			while (source.numBytesRead() < numBytes) {
				source.readChar();
				++numChars;
			}
		}
		
		list_->push(0, numChars);
		
		{
			Utf8Source source((uint8_t*)utf8, numBytes);
			for (int i = 0; i < numChars; ++i)
				list_->set(i, source.readChar());
		}
	}
}

String::String(Ref<Media> list)
	: list_(list)
{}

int String::find(String s, int i) // HACK, needs to be moved to List
{
	int n = length();
	int j = 0;
	int m = s.length();
	
	while ((i < n) && (j < m))
	{
		j = (s.get(j) == get(i)) ? j + 1 : 0;
		++i;
	}
	
	if (j != m)
		i = -1;
	else 
		i -= m;
	
	return i;
}

void String::insert(int i, String s) // HACK, needs to be moved to List
{
	int n = s.length();
	if (n > 0) {
		list_->push(i, n);
		for (int k = 0; k < n; ++k)
			list_->set(i+k, s.list_->get(k));
	}
}

int String::toInt(bool* ok)
{
	uint64_t value; int sign;
	int length = 0;
	parseInteger(&value, &sign, *this, 0, &length);
	
	if (length == 0) {
		if (ok) *ok = false;
	}
	else if (value > intMax) {
		value = 0;
		if (ok) *ok = false;
	}
	
	return sign * int(value);
}

Ref<StringList, Owner> String::split(String sep)
{
	Ref<StringList, Owner> list = new StringList;
	
	int i = 0;
	while (true) {
		int k = find(sep, i);
		if (k == -1) break;
		list->append(copy(i, k - i));
		i = k + sep.length();
	}
	
	if (i < length())
		list->append(copy(i, length() - i));
	
	return list;
}


char* String::strdup()
{
	int numChars = length();
	int numBytes = 0;
	{
		Utf8Sink nullSink;
		for (int i = 0; i < numChars; ++i)
			nullSink.writeChar(get(i));
		numBytes = nullSink.numBytesWritten();
	}
	
	char* buf = (char*)malloc(numBytes + 1);
	Utf8Sink sink((uint8_t*)buf, numBytes);
	for (int i = 0; i < numChars; ++i)
		sink.writeChar(get(i));
	buf[numBytes] = 0;
	
	return buf;
}

} // namespace pona
