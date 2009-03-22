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

#include "atoms"
#include "List.hpp"

namespace pona
{

class String;

typedef List<String> StringList;

class String: public Ref<List<Char>, Owner>
{
public:
	typedef Char Element;
	typedef List<Char> Media;
	
	String();
	String(Char ch);
	String(const char* utf8, int numBytes = -1, int numChars = -1);
	String(Ref<Media, Owner> list);
	
	inline Ref<Media> media() const { return get(); }
	
	inline bool operator<(const String& b) const { return *get() < *b.get(); }
	inline bool operator==(const String& b) const { return *get() == *b.get(); }
	inline bool operator>(const String& b) const { return *get() > *b.get(); }
	inline bool operator!=(const String& b) const { return *get() != *b.get(); }
	inline bool operator<=(const String& b) const { return *get() <= *b.get(); }
	inline bool operator>=(const String& b) const { return *get() >= *b.get(); }
	
	inline String operator+(String b) const { String s; s.get()->append(get()); s->append(b.get()); return s; }
	inline String& operator<<(String b) { get()->append(b.get()); return *this; }
	inline String& operator<<(Char ch) { get()->append(ch); return *this; }
	
	char* strdup() const;
};

String operator*(Char ch, int n);
inline String operator*(int n, Char ch) { return ch * n; }

inline bool operator==(String a, const char* b) { return a == String(b); }
inline bool operator==(const char* b, String a) { return a == b; }

} // namespace pona

#endif // PONA_STRING_HPP
