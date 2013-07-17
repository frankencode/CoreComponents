/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "LineSink.h"

namespace fkit
{

LineSink::LineSink(Stream *stream, const char *eol, int maxLineLength)
	: stream_(stream),
	  eol_(eol),
	  bufFill_(0),
	  buf_(ByteArray::create(maxLineLength))
{}

Stream *LineSink::stream() const { return stream_; }

String LineSink::prefix() const { return prefix_; }

void LineSink::setPrefix(String prefix)
{
	Ref<ByteArray> bufSaved;
	if (bufFill_ > prefix_->size())
		bufSaved = buf_->copy(prefix_->size(), bufFill_);
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

void LineSink::feed(const char *data, int size)
{
	if (size > buf_->size() - bufFill_)
		FKIT_THROW(StreamIoException, strcat("Maximum line length of ", fkit::intToStr(buf_->size()), " bytes exceeded"));
	buf_->write(bufFill_, data, size);
	bufFill_ += size;
}

void LineSink::flush()
{
	feed(eol_->data(), eol_->size());
	stream_->write(ByteRange(buf_, 0, bufFill_));
	bufFill_ = prefix_->size();
}

} // namespace fkit
