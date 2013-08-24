/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "exceptions.h"
#include "Request.h"
#include "PayloadSource.h"

namespace fnode
{

Ref<PayloadSource> PayloadSource::open(Stream *stream, Request *request)
{
	return new PayloadSource(stream, request);
}

PayloadSource::PayloadSource(Stream *stream, Request *request)
	: stream_(stream),
	  contentLength_(0),
	  bytesLeft_(0)
{
	if (request->value("Transfer-Encoding") == "chunked") {
		throw NotImplemented();
	}
	else {
		String h;
		if (request->lookup("Content-Length", &h)) {
			bool ok = true;
			contentLength_ = h->toInt64(&ok);
			if (!ok || contentLength_ < 0) throw BadRequest();
		}
		else {
			contentLength_ = 0;
		}
	}
	bytesLeft_ = contentLength_;
}

bool PayloadSource::readyRead(double interval) const
{
	return stream_->readyRead(interval) || bytesLeft_ == 0;
}

int PayloadSource::readAvail(ByteArray *buf)
{
	if (bytesLeft_ == 0) return 0;
	int h = buf->size() > bytesLeft_ ? bytesLeft_ : buf->size();
	h = stream_->readAvail(ByteRange(buf, 0, h));
	bytesLeft_ -= h;
	return h;
}

bool PayloadSource::isConsumed() const
{
	return bytesLeft_ == 0;
}

void PayloadSource::drain()
{
	if (bytesLeft_ == 0) return;
	Ref<ByteArray> buf = ByteArray::create(0x3FFF);
	while (bytesLeft_ > 0) {
		if (readAvail(buf) == 0) break;
	}
}

} // namespace fnode
