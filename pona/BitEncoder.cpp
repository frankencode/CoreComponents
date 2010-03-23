/*
 * BitEncoder.hpp -- bit-vise writing to a 'Stream'
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "strings.hpp"
#include "BitEncoder.hpp"

namespace pona
{

BitEncoder::BitEncoder(Ref<Stream> stream, int bufCapacity, int endian)
	: stream_(stream),
	  endian_(endian),
	  bufCapacity_(bufCapacity),
	  buf_(new uint8_t[bufCapacity]),
	  i_(0),
	  iBit_(0),
	  nw_(0)
{
	pona::bzero(buf_, bufCapacity_);
}

BitEncoder::BitEncoder(void* buf, int bufCapacity, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapacity_(bufCapacity),
	  buf_((uint8_t*)buf),
	  i_(0),
	  iBit_(0),
	  nw_(0)
{
	pona::bzero(buf_, bufCapacity_);
}

BitEncoder::~BitEncoder()
{
	if (stream_)
	{
		flush();
		delete[] buf_;
		buf_ = 0;
	}
}

void BitEncoder::flush()
{
	if (!stream_)
		PONA_THROW(StreamIoException, "Output buffer exhausted");
	
	int h = i_ + (iBit_ > 0);
	if (h > 0)
	{
		stream_->write((void*)buf_, h);
		pona::bzero((void*)buf_, h);
		nw_ += h;
		i_ = 0;
		iBit_ = 0;
	}
}

} // namespace pona
