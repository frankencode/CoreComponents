/*
 * ByteSource.cpp -- byte-vise reading from a 'Stream'
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "ByteSource.hpp"

namespace pona
{

ByteSource::ByteSource(Ref<Stream> stream, int bufCapa, int endian)
	: stream_(stream),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(0),
	  buf_(new uint8_t[bufCapa_]),
	  i_(0),
	  nr_(0)
{
	fill();
}

ByteSource::ByteSource(const void* buf, int bufCapa, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(bufCapa),
	  buf_(reinterpret_cast<uint8_t*>(const_cast<void*>(buf))),
	  i_(0),
	  nr_(0)
{}

ByteSource::~ByteSource()
{
	if (stream_) {
		delete[] buf_;
		buf_ = 0;
	}
}

void ByteSource::fill()
{
	if (!stream_)
		PONA_THROW(StreamIoException, "Input buffer exhausted");

	bufFill_ = stream_->readAvail(buf_, bufCapa_);
	if (bufFill_ == 0)
		PONA_THROW(StreamIoException, "Reading beyond end of input");
	
	i_ = 0;
}

} // namespace pona
