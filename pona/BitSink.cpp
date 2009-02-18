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

#include <string.h>
#include "BitSink.hpp"

namespace pona
{

BitSink::BitSink(Ref<Stream> stream, int bufCapacity, int endian)
	: stream_(stream),
	  endian_(endian),
	  bufCapacity_(bufCapacity),
	  buf_(new uint8_t[bufCapacity]),
	  i_(0),
	  iBit_(0),
	  nw_(0)
{
	::memset(buf_, 0, bufCapacity_);
}

BitSink::BitSink(uint8_t* buf, int bufCapacity, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapacity_(bufCapacity),
	  buf_(buf),
	  i_(0),
	  iBit_(0),
	  nw_(0)
{
	::memset(buf_, 0, bufCapacity_);
}

BitSink::~BitSink()
{
	if (stream_)
	{
		flush();
		delete[] buf_;
		buf_ = 0;
	}
}

void BitSink::flush()
{
	if (!stream_)
		PONA_THROW(StreamIoException, "Output buffer exhausted");
	
	int h = i_ + (iBit_ > 0);
	if (h > 0)
	{
		stream_->write((void*)buf_, h);
		::memset((void*)buf_, 0, h);
		nw_ += h;
		i_ = 0;
		iBit_ = 0;
	}
}

} // namespace pona
