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

#include "BitSource.hpp"

namespace pona
{

BitSource::BitSource(Ref<Stream> stream, int bufCapa, int endian)
	: stream_(stream),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(0),
	  buf_(new uint8_t[bufCapa]),
	  i_(0),
	  iBit_(8),
	  nr_(0)
{
	fill();
}

BitSource::BitSource(uint8_t* buf, int bufCapa, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(bufCapa),
	  buf_(buf),
	  i_(0),
	  iBit_(0),
	  nr_(bufCapa)
{}

BitSource::~BitSource()
{
	if (stream_)
	{
		delete[] buf_;
		buf_ = 0;
	}
}

void BitSource::fill()
{
	if (!stream_)
		PONA_THROW(StreamIoException, "Input buffer exhausted");

	bufFill_ = stream_->readAvail(buf_, bufCapa_);
	if (bufFill_ == 0)
		PONA_THROW(StreamIoException, "Reading beyond end of input");
	
	nr_ += bufFill_;
	i_ = 0;
	iBit_ = 0;
}

} // namespace pona
