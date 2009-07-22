/*
 * BitSink.hpp -- bit-vise writing to a 'Stream'
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

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

BitSink::BitSink(void* buf, int bufCapacity, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapacity_(bufCapacity),
	  buf_((uint8_t*)buf),
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
