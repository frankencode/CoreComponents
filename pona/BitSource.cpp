/*
 * BitSoruce.hpp -- bit-vise reading from a 'Stream'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

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

BitSource::BitSource(void* buf, int bufCapa, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(bufCapa),
	  buf_((uint8_t*)buf),
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
