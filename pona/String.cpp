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
	: Ref<List<Char>, Owner>(new Media)
{}

String::String(Char ch)
	: Ref<List<Char>, Owner>(new Media)
{
	instance_->append(ch);
}

String::String(const char* utf8, int numBytes, int numChars)
	: Ref<List<Char>, Owner>(new Media)
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
		
		instance_->push(0, numChars);
		
		{
			Utf8Source source((uint8_t*)utf8, numBytes);
			for (int i = 0; i < numChars; ++i)
				instance_->set(i, source.readChar());
		}
	}
}

String::String(Ref<Media, Owner> list)
	: Ref<List<Char>, Owner>(list)
{
	if (!instance_) set(new Media);
}

char* String::strdup() const
{
	int numChars = instance_->length();
	int numBytes = 0;
	{
		Utf8Sink nullSink;
		for (int i = 0; i < numChars; ++i)
			nullSink.writeChar(instance_->get(i));
		numBytes = nullSink.numBytesWritten();
	}
	
	char* buf = (char*)malloc(numBytes + 1);
	Utf8Sink sink((uint8_t*)buf, numBytes);
	for (int i = 0; i < numChars; ++i)
		sink.writeChar(instance_->get(i));
	buf[numBytes] = 0;
	
	return buf;
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

} // namespace pona
