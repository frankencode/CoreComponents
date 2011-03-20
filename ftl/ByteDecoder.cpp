/*
 * ByteDecoder.cpp -- byte-vise reading from a 'Stream'
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "ByteDecoder.hpp"

namespace ftl
{

ByteDecoder::ByteDecoder(Ref<Stream> stream, int bufCapa, int endian)
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

ByteDecoder::ByteDecoder(const void* buf, int bufCapa, int endian)
	: stream_(0),
	  endian_(endian),
	  bufCapa_(bufCapa),
	  bufFill_(bufCapa),
	  buf_(reinterpret_cast<uint8_t*>(const_cast<void*>(buf))),
	  i_(0),
	  nr_(0)
{}

ByteDecoder::~ByteDecoder()
{
	if (stream_) {
		delete[] buf_;
		buf_ = 0;
	}
}

void ByteDecoder::fill()
{
	if (!stream_)
		FTL_THROW(EncodingException, "Unexpected end of input");

	bufFill_ = stream_->readAvail(buf_, bufCapa_);
	if (bufFill_ == 0)
		FTL_THROW(EncodingException, "Unexpected end of input");
	
	i_ = 0;
}

} // namespace ftl
