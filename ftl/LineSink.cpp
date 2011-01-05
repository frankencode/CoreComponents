/*
 * LineSink.cpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include "LineSink.hpp"

namespace ftl
{

LineSink::LineSink(Ref<Stream> stream, int bufCapa, const char* eol)
	: stream_(stream),
	  eol_(eol),
	  bufCapa_(bufCapa),
	  buf_(new uint8_t[bufCapa])
{}

LineSink::~LineSink()
{
	delete[] buf_;
	buf_ = 0;
}

void LineSink::writeLine(String line)
{
	int n = line->size();
	int n2 = eol_->size();
	
	if (n + n2 > bufCapa_)
		FTL_THROW(StreamIoException, "Output buffer exhausted");
	
	for (int i = 0; i < n; ++i)
		buf_[i] = line->at(i);
	
	for (int i = 0; i < n2; ++i)
		buf_[n + i] = eol_->at(i);

	stream_->write(buf_, n + n2);
}

void LineSink::write(String s)
{
	stream_->write(s->data(), s->size());
}

Ref<Stream> LineSink::stream() const
{
	return stream_;
}

} // namespace ftl
