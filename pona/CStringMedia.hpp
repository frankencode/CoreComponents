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

#ifndef PONA_CSTRINGMEDIA_HPP
#define PONA_CSTRINGMEDIA_HPP

#include "RandomAccessMedia.hpp"

namespace pona
{

class CStringMedia: public RandomAccessMedia<char>
{
public:
	typedef char Element;

	CStringMedia(const char* s, int i = 0, int n = -1)
		: sr_(s + i),
		  sw_(0),
		  len_(n)
	{
		if (len_ == -1)
		{
			len_ = 0;
			if (s)
				while (s[len_] != 0) { ++len_; }
		}
	}
	
	CStringMedia(char* s, int i = 0, int n = -1)
		: sr_(s + i),
		  sw_(s),
		  len_(n)
	{
		if (len_ == -1)
		{
			len_ = 0;
			if (s)
				while (s[len_] != 0) { ++len_; }
		}
	}
	
	inline char get(int i) { return sr_[i]; }
	inline void set(int i, char e) { sw_[i] = e; }
	inline int length() const { return len_; }
	
private:
	const char* sr_;
	char* sw_;
	int len_;
};

} // namespace pona

#endif // PONA_CSTRINGMEDIA_HPP
