/*
 * LineSink.cpp -- canonically buffered data sink
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "LineSink.hpp"

namespace ftl
{

LineSink::LineSink(Ref<Stream> stream, const char* eol, int maxLineLength)
	: stream_(stream),
	  eol_(eol),
	  bufFill_(0),
	  buf_(maxLineLength)
{}

Ref<Stream> LineSink::stream() const { return stream_; }

String LineSink::prefix() const { return prefix_; }

void LineSink::setPrefix(String prefix)
{
	Ref<ByteArray, Owner> bufSaved;
	if (bufFill_ > prefix_->size())
		bufSaved = buf_.copy(prefix_->size(), bufFill_);
	bufFill_ = 0;
	prefix_ = prefix;
	if (prefix_->size() > 0)
		feed(prefix_->data(), prefix_->size());
	if (bufSaved)
		feed(bufSaved->data(), bufSaved->size());
}

void LineSink::writeLine(String line)
{
	write(line);
	flush();
}

void LineSink::write(String text)
{
	int i = 0, j = 0;
	while ((j = text->find(i, '\n')) != text->size()) {
		feed(text->data() + i, j - i);
		flush();
		i = j + 1;
	}
	if (i < j)
		feed(text->data() + i, j - i);
}

void LineSink::feed(const char* data, int size)
{
	if (size > buf_.size() - bufFill_)
		FTL_THROW(StreamIoException, str::cat("Maximum line length of ", ftl::intToStr(buf_.size()), " bytes exceeded"));
	buf_.write(bufFill_, data, size);
	bufFill_ += size;
}

void LineSink::flush()
{
	feed(eol_->data(), eol_->size());
	stream_->write(buf_.data(), bufFill_);
	bufFill_ = prefix_->size();
}

} // namespace ftl
