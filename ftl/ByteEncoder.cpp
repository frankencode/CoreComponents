/*
 * ByteEncoder.cpp -- byte-vise writing to a 'Stream'
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "ByteEncoder.hpp"

namespace ftl
{

ByteEncoder::ByteEncoder()
	: endian_(FTL_DEFAULT_ENDIAN),
	  bufCapa_(-1),
	  buf_(0),
	  i_(0),
	  nw_(0)
{}

ByteEncoder::ByteEncoder(Ref<Stream> stream, int bufCapacity, int endian)
	: stream_(stream),
	  endian_(endian),
	  bufCapa_(bufCapacity),
	  buf_(new uint8_t[bufCapa_]),
	  i_(0),
	  nw_(0)
{}

ByteEncoder::ByteEncoder(void* buf, int bufCapacity, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapa_(bufCapacity),
	  buf_(reinterpret_cast<uint8_t*>(buf)),
	  i_(0),
	  nw_(0)
{}

ByteEncoder::~ByteEncoder()
{
	if ((stream_) && (buf_))
	{
		if (i_ > 0)
			flush();
		delete[] buf_;
		buf_ = 0;
	}
}

void ByteEncoder::flush()
{
	if (!stream_)
		FTL_THROW(StreamIoException, "Output buffer exhausted");
	stream_->write((void*)buf_, i_);
	nw_ += i_;
	i_ = 0;
}

} // namespace ftl
