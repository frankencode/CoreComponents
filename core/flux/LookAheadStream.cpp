/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "LookAheadStream.h"

namespace flux
{

Ref<LookAheadStream> LookAheadStream::open(Stream *source, int windowSize)
{
	return new LookAheadStream(source, windowSize);
}

LookAheadStream::LookAheadStream(Stream *source, int windowSize)
	: source_(source),
	  window_(ByteArray::create(windowSize)),
	  w_(windowSize), m_(0), i_(0),
	  done_(false)
{}

bool LookAheadStream::readyRead(double interval) const
{
	if (w_ <= i_) {
		if (done_) return source_->readyRead(interval);
		return true;
	}
	if (i_ < m_) return true;
	return source_->readyRead(interval);
}

int LookAheadStream::read(ByteArray *buf)
{
	if (w_ <= i_) {
		if (done_) return source_->read(buf);
		return 0;
	}

	if (i_ < m_) {
		int i1 = i_ + buf->size();
		if (i1 > m_) i1 = m_;
		*buf = *ByteRange(window_, i_, i1);
		int n = i1 - i_;
		i_ = i1;
		return n;
	}

	int r = buf->size();
	if (m_ + r > w_) r = w_ - m_;
	int n = source_->read(ByteRange(buf, 0, r));
	if (n == 0) return 0;
	*ByteRange(window_, m_, m_ + n) = *buf;
	m_ += n;
	i_ += n;
	return n;
}

off_t LookAheadStream::transfer(off_t count, Stream *sink, ByteArray *buf)
{
	if (done_) return source_->transfer(count, sink, buf);
	return Stream::transfer(count, sink, buf);
}

void LookAheadStream::replay()
{
	i_ = 0;
	done_ = false;
}

void LookAheadStream::done()
{
	done_ = true;
}

} // namespace flux
