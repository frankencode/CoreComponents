/*
 * BitEncoder.hpp -- bit-vise writing to a 'Stream'
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "strings.hpp"
#include "BitEncoder.hpp"

namespace ftl
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
	mem::clr(buf_, bufCapacity_);
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
	mem::clr(buf_, bufCapacity_);
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
		FTL_THROW(StreamIoException, "Output buffer exhausted");
	
	int h = i_ + (iBit_ > 0);
	if (h > 0)
	{
		stream_->write((void*)buf_, h);
		mem::clr((void*)buf_, h);
		nw_ += h;
		i_ = 0;
		iBit_ = 0;
	}
}

} // namespace ftl
