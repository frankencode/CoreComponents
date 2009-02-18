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

#ifndef PONA_LINEPRINTER_HPP
#define PONA_LINEPRINTER_HPP

#include "Atoms.hpp"
#include "String.hpp"

#ifdef arg
#undef arg
#endif

namespace pona
{

/** \todo Allow to pass the fill character for printing
  */
class LinePrinter
{
public:
	LinePrinter(String format);
	
	LinePrinter& arg(const char* value);
	LinePrinter& arg(String value);
	LinePrinter& arg(Char value);
	LinePrinter& arg(bool value);
	LinePrinter& arg(uint64_t value);
	LinePrinter& arg(int64_t value);
	LinePrinter& arg(float64_t value);
	
	inline LinePrinter& arg(char value) { return arg(Char(value)); }
	inline LinePrinter& arg(wchar_t value) { return arg(Char(value)); }
	inline LinePrinter& arg(uint8_t value) { return arg(uint64_t(value)); }
	inline LinePrinter& arg(uint16_t value) { return arg(uint64_t(value)); }
	inline LinePrinter& arg(int16_t value) { return arg(int64_t(value)); }
	inline LinePrinter& arg(int32_t value) { return arg(int64_t(value)); }
	inline LinePrinter& arg(uint32_t value) { return arg(uint64_t(value)); }
	inline LinePrinter& arg(void* value) { return arg(uint64_t(value)); }
	
#ifdef _MSC_VER
	inline LinePrinter& arg(long unsigned value) { return arg(uint64_t(value)); }
	inline LinePrinter& arg(long int value) { return arg(int64_t(value)); }
#endif
	
	inline LinePrinter& operator%(void* value) { return arg(value); }
	
	template<class T>
	inline LinePrinter& operator%(T value) { return arg(value); }
	
	inline operator String() const { return line_.copy(0, linePos_); }
	
private:
	void putChar(char ch);

	void nextFormat();
	static double trunc(double x);
	static double round(double x);
	void printFloatingPoint(float64_t value);
	void printInteger(uint64_t x, int sign);
	void printString(const char* value);
	void printString(String value);
	
	String format_;
	int formatPos_;
	
	enum { lineChunkSize = 16 };
	mutable String line_;
	String lineChunk_;
	int linePos_;
	
	bool hasNextFormat_;
	int wi_, wf_;
	int base_;
};

inline LinePrinter format(String s) { return LinePrinter(s); }

} // namespace pona

#endif // PONA_LINEPRINTER_HPP
