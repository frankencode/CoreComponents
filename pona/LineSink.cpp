/*
 * LineSink.cpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include "LineSink.hpp"

namespace pona
{

LineSink::LineSink(Ref<Stream> stream, int bufCapa, String eol)
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
	int n = line->length();
	int n2 = eol_->length();

	if (n + n2 > bufCapa_)
		PONA_THROW(StreamIoException, "Output buffer exhausted");

	for (int i = 0; i < n; ++i)
		buf_[i] = line->get(i);

	for (int i = 0; i < n2; ++i)
		buf_[n + i] = eol_->get(i);

	stream_->write(buf_, n + n2);
}

void LineSink::write(String buf)
{
	CString utf8 = buf->utf8();
	stream_->write(utf8, utf8->length() - 1);
}

Ref<Stream> LineSink::stream() const
{
	return stream_;
}

} // namespace pona
