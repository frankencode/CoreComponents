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

#include "Vector.hpp"
#include "Stack.hpp"
#include "NumberFormatting.hpp"
#include "SyntaxFactory.hpp"
#include "FormatSpecifier.hpp"
#include "LinePrinter.hpp"

namespace pona
{

LinePrinter::LinePrinter(String format)
	: format_(format),
	  formatPos_(0),
	  line_(lineChunkSize),
	  linePos_(0)
{
	nextFormat();
}

inline void LinePrinter::putChar(char ch)
{
	if (linePos_ == line_.length())
		line_.list()->push(linePos_, lineChunkSize);
	line_.set(linePos_, ch);
	++linePos_;
}

void LinePrinter::nextFormat()
{
	if (formatPos_ == format_.length()) return;
	
	Ref<FormatSpecifier> specifier = syntaxFactory()->formatSpecifier();
	
	wi_ = 0;
	wf_ = 0;
	base_ = 10;
	
	int i0 = formatPos_;
	int i1 = 0;
	hasNextFormat_ = specifier->find(format_, &i0, &i1, &wi_, &wf_, &base_);
	
	if (!hasNextFormat_)
	{
		for (int i = formatPos_, n = format_.length(); i < n; ++i)
			putChar(format_.get(i));
		
		formatPos_ = format_.length();
	}
	else
	{
		for (int i = formatPos_; i < i0; ++i)
			putChar(format_.get(i));
		
		formatPos_ = i1;
	}
}

LinePrinter& LinePrinter::arg(const char* value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");
	
	printString(value);
	nextFormat();
	return *this;
}

LinePrinter& LinePrinter::arg(String value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");
	
	printString(value);
	nextFormat();
	return *this;
}

LinePrinter& LinePrinter::arg(Char value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");
	putChar(char(value));   // HACK, UTF8 conversion would be required here
	nextFormat();
	return *this;
}

LinePrinter& LinePrinter::arg(bool value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");

	if (value)
		printString("true");
	else
		printString("false");
	nextFormat();
	return *this;
}

LinePrinter& LinePrinter::arg(uint64_t value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");
	
	printInteger(value, 1);
	nextFormat();
	return *this;
}

LinePrinter& LinePrinter::arg(int64_t value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");
	
	int sign = 1;
	if (value < 0) {
		sign = -1;
		value = -value;
	}
	printInteger(value, sign);
	
	nextFormat();
	return *this;
}

LinePrinter& LinePrinter::arg(float64_t value)
{
	if (!hasNextFormat_)
		PONA_THROW(FormattingException, "Unexpected end of format string");
	
	printFloatingPoint(value);
	nextFormat();
	return *this;
}

void LinePrinter::printFloatingPoint(float64_t value)
{
	String s = pona::printFloatingPoint(value, base_, wi_, wf_);
	for (int i = 0, n = s.length(); i < n; ++i)
		putChar(s.get(i));
}

void LinePrinter::printInteger(uint64_t value, int sign)
{
	String s = pona::printInteger(value, sign, base_, wi_);
	for (int i = 0, n = s.length(); i < n; ++i)
		putChar(s.get(i));
}

inline int len(const char* s)
{
	int i = 0;
	if (s != 0)
		while (*(s+i)) ++i;
	return i;
}

void LinePrinter::printString(const char* value)
{
	int n = len(value);
	for (int i = n; i < wi_; ++i)
		putChar(' ');
	for (int i = 0; i < n; ++i)
		putChar(value[i]);
}

void LinePrinter::printString(String value)
{
	int n = value.length();
	for (int i = n; i < wi_; ++i)
		putChar(' ');
	for (int i = 0; i < n; ++i)
		putChar(value.get(i));
}

} // namespace pona
