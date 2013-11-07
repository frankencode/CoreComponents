/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <flux/TimeoutLimiter.h>
#include <flux/TransferLimiter.h>
#include <flux/System.h>
#include "exceptions.h"
#include "RequestStream.h"

namespace fluxnode
{

Ref<RequestStream> RequestStream::open(Stream *stream)
{
	return new RequestStream(stream);
}

RequestStream::RequestStream(Stream *stream)
	: stream_(stream),
	  bytesLeft_(-1),
	  nlCount_(0),
	  eoi_(false),
	  chunked_(false)
{}

void RequestStream::setupTimeout(double interval)
{
	stream_ = TimeoutLimiter::open(stream_, System::now() + interval);
}

bool RequestStream::isPayloadConsumed() const
{
	return bytesLeft_ == 0;
}

void RequestStream::nextHeader()
{
	if (eoi_) throw CloseRequest();
	if (bytesLeft_ > 0) throw CloseRequest();
	bytesLeft_ = -1;
	nlCount_ = 0;
	nlMax_ = 2;
}

void RequestStream::nextPayload(int64_t length)
{
	if (eoi_) throw CloseRequest();
	bytesLeft_ = length;
	chunked_ = false;
}

void RequestStream::nextLine()
{
	if (eoi_) throw CloseRequest();
	bytesLeft_ = -1;
	nlCount_ = 0;
	nlMax_ = 1;
}

void RequestStream::nextChunk()
{
	nextLine();
	String line = readAll();
	bytesLeft_ = 0;
	for (int i = 0; i < line->size(); ++i) {
		char ch = line->at(i);
		if (ch == '\r' || ch == '\n') continue;
		bytesLeft_ *= 16;
		if ('0' <= ch && ch <= '9') bytesLeft_ += ch - '9';
		else if ('a' <= ch && ch <= 'f') bytesLeft_ += ch - 'a';
		else if ('A' <= ch && ch <= 'F') bytesLeft_ += ch - 'A';
		else throw BadRequest();
	}
	if (bytesLeft_ == 0) {
		chunked_ = false;
		nextHeader();
		nlCount_ = 1;
		TransferLimiter::open(this, 0x10000)->drain();
	}
	else {
		chunked_ = true;
	}
}

bool RequestStream::readyRead(double interval) const
{
	if (eoi_) return true;
	if (pending_) return true;
	return stream_->readyRead(interval);
}

int RequestStream::read(ByteArray *buf)
{
	if (eoi_) return 0;
	if (bytesLeft_ == 0) return 0;
	if (pending_) {
		*buf = *pending_;
		if (buf->size() < pending_->size()) {
			pending_->truncate(buf->size(), pending_->size());
			return buf->size();
		}
		int h = pending_->size();
		pending_ = 0;
		return h;
	}

	int n = stream_->read(buf);
	if (n == 0) {
		eoi_ = true;
		return 0;
	}
	if (bytesLeft_ == -1) {
		int i = 0;
		while (i < n && nlCount_ < nlMax_) {
			int i0 = i;
			i = buf->find('\n', i0);
			if (i == i0) {
				++i;
				++nlCount_;
			}
			else {
				nlCount_ = 0;
			}
		}
		if (nlCount_ == nlMax_) {
			pending_ = buf->copy(i, n);
			bytesLeft_ = 0;
			return i;
		}
		return n;
	}
	if (bytesLeft_ < n) {
		pending_ = buf->copy(bytesLeft_, n);
		int h = bytesLeft_;
		bytesLeft_ = 0;
		if (chunked_) nextChunk();
		return h;
	}
	return n;
}

void RequestStream::write(const ByteArray *buf)
{
	stream_->write(buf);
}

void RequestStream::write(const StringList *parts)
{
	stream_->write(parts);
}

} // namespace fluxnode
