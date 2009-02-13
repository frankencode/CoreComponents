/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_STRING_HPP
#define PONA_STRING_HPP

#include "Atoms.hpp"
#include "List.hpp"

namespace pona
{

class String;

typedef List<String> StringList;
	
class String
{
public:
	typedef Char Element;
	typedef List<Char> Media;
	
	String() {}
	String(int n);
	String(int n, Char ch);
	String(const char* s);
	String(const char* s, int n);
	String(const wchar_t* s);
	String(Ref<Media> list): list_(list) {}
	
	inline int length() const { return (list_) ? list_->fill() : 0; }
	
	inline void set(int i, Char ch) { list_->set(i, ch); }
	inline Char get(int i) { return list_->get(i); }
	inline Char saveGet(int i) const { return (list_) ? ((i < list_->fill()) ? list_->get(i) : Char(0)) : Char(0); }
	
	bool operator<(const String& b) const;
	bool operator<=(const String& b) const;
	bool operator>(const String& b) const;
	bool operator>=(const String& b) const;
	bool operator==(const String& b) const;
	bool operator!=(const String& b) const;
	
	/*bool operator==(const char* b) const;
	bool operator!=(const char* b) const;*/
	
	int find(String s, int i = 0);
	
	inline String range(int i, int n) { return String(list_->range(i, n)); }
	inline String copy(int i, int n) { return String(list_->copy(i, n)); }
	inline String copy() { return copy(0, length()); }
	inline void del(int i, int n = 1) { list_->pop(i, n); }
	void paste(int i, String s);
	void paste(int i, Char ch);
	
	inline String append(String b) { paste(length(), b); return *this; }
	inline String append(Char ch) { paste(length(), ch); return *this; }
	
	inline String operator+(String b) const { return String().append(*this).append(b); }
	inline String operator+=(String b) { return append(b); }
	inline String operator+=(Char ch) { return append(ch); }
	
	inline Ref<Media> list() const { return list_; }

	int toInt(bool* ok = 0);
	
	Ref<StringList, Owner> split(String sep);
	
	//----------------------------------------------------------------
	// conversion to zero-terminated C strings
	//----------------------------------------------------------------

	char* strdup();
	wchar_t* wcsdup();
	
private:
	void init(const char* s);
	int eqc(const String& b) const;

	mutable Ref<Media, Owner> list_;
};

inline char* exceptionMessageToUtf8(String s) { return s.strdup(); }

} // namespace pona

#endif // PONA_STRING_HPP

