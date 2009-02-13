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

#include "NumberFormatting.hpp"
#include "String.hpp"

namespace pona
{

inline int len(const char* s)
{
	int i = 0;
	if (s != 0)
		while (*(s+i)) ++i;
	return i;
}

inline int len(const wchar_t* s)
{
	int i = 0;
	if (s != 0)
		while (*(s+i)) ++i;
	return i;
}

String::String(int n)
	: list_(new Media)
{
	list_->push(0, n);
}

String::String(int n, Char ch)
	: list_(new Media)
{
	list_->push(0, n);
	for (int i = 0; i < n; ++i)
		list_->set(i, ch);
}

String::String(const char* s)
	: list_(new Media)
{
	int n = len(s);
	if (n > 0)
	{
		list_->push(0, n);
		for (int i = 0; i < n; ++i)
			list_->set(i, (Char)s[i]);
	}
}

String::String(const char* s, int n)
	: list_(new Media)
{
	if (n > 0)
	{
		list_->push(0, n);
		for (int i = 0; i < n; ++i)
			list_->set(i, (Char)s[i]);
	}
}

String::String(const wchar_t* s)
	: list_(new Media)
{
	int n = len(s);
	if (n > 0)
	{
		list_->push(0, n);
		for (int i = 0; i < n; ++i)
			list_->set(i, (Char)s[i]);
	}
}

inline int String::eqc(const String& b) const
{
	if (!list_) return 0;
	int i = 0;
	int n = list_->fill();
	if (n < b.list_->fill()) n = b.list_->fill();
	while ((i < n) && (saveGet(i) == b.saveGet(i)))
		++i;
	return i;
}

bool String::operator<(const String& b) const
{
	int ne = eqc(b); // equal count
	return saveGet(ne) < b.saveGet(ne);
}

bool String::operator<=(const String& b) const
{
	int ne = eqc(b); // equal count
	return saveGet(ne) < b.saveGet(ne);
}

bool String::operator>(const String& b) const
{
	int ne = eqc(b); // equal count
	return saveGet(ne) < b.saveGet(ne);
}

bool String::operator>=(const String& b) const
{
	int ne = eqc(b); // equal count
	return saveGet(ne) < b.saveGet(ne);
}

bool String::operator==(const String& b) const
{
	if (length() == b.length())
	{
		if (length() == 0)
			return list() == b.list();
		else
			return eqc(b) == length();
	}
	return false;
}

bool String::operator!=(const String& b) const
{
	return !(*this == b);
}

/*bool String::operator==(const char* b) const
{
	int i = 0;
	int n = length();
	
	while ((*b) && (*))
	{
		if (get(i) != (Char)*b)
			return false;
		++i;
		++b;
	}
	
	return (i == length());
}

bool String::operator!=(const char* b) const
{
	return !(*this == b);
}
*/

int String::find(String s, int i)
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

void String::paste(int i, String s)
{
	int n = s.length();
	if (n > 0)
	{
		if (!list_) list_ = new Media;
		list_->push(i, n);
		for (int k = 0; k < n; ++k)
			list_->set(i+k, s.get(k));
	}
}

void String::paste(int i, Char ch)
{
	if (!list_) list_ = new Media;
	list_->pushBack(ch);
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
	while (true)
	{
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
	int n = length();
	char* s = (char*)malloc(n+1);
	for (int i = 0; i < n; ++i)
		s[i] = char(get(i));
	s[n] = 0;
	return s;
}

wchar_t* String::wcsdup()
{
	int n = length();
	wchar_t* s = (wchar_t*)malloc(sizeof(wchar_t) * (n + 1));
	for (int i = 0; i < n; ++i)
		s[i] = wchar_t(get(i));
	s[n] = 0;
	return s;
}

} // namespace pona
