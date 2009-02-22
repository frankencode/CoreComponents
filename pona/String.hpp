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
	
	String();
	String(int n);
	String(int n, Char ch);
	String(const char* utf8, int numBytes = -1, int numChars = -1);
	String(Ref<Media> list);
	
	inline Ref<Media> media() const { return list_; }
	inline int length() const { return list_->fill(); }
	
	inline Char get(int i) const { return list_->get(i); }
	inline void set(int i, Char ch) { list_->set(i, ch); }
	inline Char saveGet(int i) const { return (i < list_->fill()) ? list_->get(i) : Char(0); }
	
	inline bool operator<(const String& b) const { return *list_ < *b.list_; }
	inline bool operator==(const String& b) const { return *list_ == *b.list_; }
	inline bool operator>(const String& b) const { return *list_ > *b.list_; }
	inline bool operator!=(const String& b) const { return *list_ != *b.list_; }
	inline bool operator<=(const String& b) const { return *list_ <= *b.list_; }
	inline bool operator>=(const String& b) const { return *list_ >= *b.list_; }
	
	int find(String s, int i = 0);
	
	inline String range(int i, int n) { return String(list_->range(i, n)); }
	inline String copy(int i, int n) { return String(list_->copy(i, n)); }
	inline String copy() { return copy(0, length()); }
	inline void remove(int i, int n = 1) { list_->pop(i, n); }
	void insert(int i, String s);
	inline void insert(int i, Char ch) { list_->push(i, 1, &ch); }
	
	inline String append(String b) { insert(length(), b); return *this; }
	inline String append(Char ch) { insert(length(), ch); return *this; }
	
	inline String operator+(String b) const { return String().append(*this).append(b); }
	inline String operator+=(String b) { return append(b); }
	inline String operator+=(Char ch) { return append(ch); }
	
	int toInt(bool* ok = 0);
	
	Ref<StringList, Owner> split(String sep);
	
	char* strdup();
	
private:
	Ref<Media, Owner> list_;
};

} // namespace pona

#endif // PONA_STRING_HPP

