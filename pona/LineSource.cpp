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

#include "LineSource.hpp"

namespace pona
{

LineSource::LineSource(Ref<Stream> stream, int bufCapa, String eol)
	: stream_(stream),
	  eol_(eol),
	  cacheFillLines_(0),
	  cache_(bufCapa),
	  bufCapa_(bufCapa),
	  buf_(new char[bufCapa])
{}

LineSource::~LineSource()
{
	delete[] buf_;
	buf_ = 0;
}

String LineSource::readLine()
{
	while (cacheFillLines_ == 0)
	{
		int bufFill = stream_->readAvail(buf_, bufCapa_);
		
		if (bufFill == 0)
			return String();
		
		for (int i = 0, nk = eol_.length(), k = 0; i < bufFill; ++i)
		{
			if (cache_.fill() == cache_.size())
				PONA_THROW(StreamIoException, "Input buffer exhausted");

			Char ch = buf_[i];
			k = (ch == eol_.get(k)) ? k + 1 : 0;
		
			if (k == nk)
			{
				k = 0;
				++cacheFillLines_;
			}
			
			cache_.pushBack(ch);
		}
	}

	int nk = eol_.length();
	int k = 0;
	int i = 0;
	while (k < nk)
	{
		Char ch = cache_.popFront();
		k = (ch == eol_.get(k)) ? k + 1 : 0;
		buf_[i] = ch;
		++i;
	}

	--cacheFillLines_;
	
	return String(buf_, i - nk);
}

Ref<Stream> LineSource::stream() const
{
	return stream_;
}

} // namespace pona
