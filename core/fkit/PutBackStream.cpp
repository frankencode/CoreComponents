/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "Mutex.h"
#include "Guard.h"
#include "PutBackStream.h"

namespace fkit
{

Ref<PutBackStream> PutBackStream::open(Stream *stream)
{
	return new PutBackStream(stream);
}

PutBackStream::PutBackStream(Stream *stream)
	: stream_(stream),
	  mutex_(Mutex::create())
{}

bool PutBackStream::readyRead(double interval) const
{
	if (pending_) return true;
	return stream_->readyRead(interval);
}

int PutBackStream::read(ByteArray *buf)
{
	if (pending_) {
		*buf = *pending_;
		if (buf->size() < pending_->size()) {
			pending_->truncate(buf->size(), pending_->size());
			return buf->size();
		}
		else {
			int h = pending_->size();
			pending_ = 0;
			return h;
		}
	}
	return stream_->read(buf);
}

void PutBackStream::write(const ByteArray *buf)
{
	stream_->write(buf);
}

void PutBackStream::write(const StringList *parts)
{
	stream_->write(parts);
}

void PutBackStream::putBack(ByteArray *pending)
{
	Guard<Mutex> guard(mutex_);
	pending_ = pending;
	if (pending_) {
		if (pending_->size() == 0)
			pending_ = 0;
	}
}

} // namespace fkit