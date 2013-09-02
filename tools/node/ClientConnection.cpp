/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <fkit/TransferLimiter.h>
#include <fkit/TimeoutLimiter.h>
#include <fkit/LineSource.h>
#include <fkit/System.h>
#include <fkit/StreamTap.h>
#include <fkit/LineBuffer.h>
#include "exceptions.h"
#include "ErrorLog.h"
#include "TapBuffer.h"
#include "RequestStream.h"
#include "ClientConnection.h"

namespace fnode
{

Ref<ClientConnection> ClientConnection::create(StreamSocket *socket)
{
	return new ClientConnection(socket);
}

ClientConnection::ClientConnection(StreamSocket *socket)
	: requestStream_(RequestStream::open(socket)),
	  stream_(requestStream_),
	  address_(SocketAddress::getRemoteAddress(socket))
{
	if (errorLog()->level() >= DebugLogLevel) {
		Ref<Stream> requestBuffer =TapBuffer::open(errorLog()->debugStream(), address_->networkAddress() + " > ");
		Ref<Stream> responseBuffer = TapBuffer::open(errorLog()->debugStream(), address_->networkAddress() + " < ");
		stream_ = StreamTap::open(stream_, requestBuffer, responseBuffer);
	}
}

Ref<Request> ClientConnection::readRequest()
{
	if (pendingRequest_) {
		Ref<Request> h = pendingRequest_;
		pendingRequest_ = 0;
		return h;
	}
	request_ = 0;
	request_ = scanRequest();
	return request_;
}

void ClientConnection::putBack(Request *request)
{
	pendingRequest_ = request;
}

void ClientConnection::setupTimeout(double interval)
{
	requestStream_->setupTimeout(interval);
}

bool ClientConnection::isPayloadConsumed() const
{
	return requestStream_->isPayloadConsumed();
}

Ref<Request> ClientConnection::scanRequest()
{
	requestStream_->nextHeader();

	Ref<Request> request = Request::create();
	request->payload_ = stream_;
	request->time_ = System::now();

	try {
		Ref<TransferLimiter> limiter = TransferLimiter::open(stream_, 0x10000);
		Ref<ByteArray> buf = ByteArray::create(0xFFF);
		Ref<LineSource> source = LineSource::open(limiter, buf);

		String line;

		if (!source->read(&line)) throw CloseRequest();
		request->line_ = line;

		if (line->count(' ') != 2) throw BadRequest();
		{
			int i0 = 0, i1 = line->find(' ');
			request->method_ = line->copy(i0, i1);
			i0 = i1 + 1; i1 = line->find(' ', i0);
			request->target_ = line->copy(i0, i1);
			request->version_ = line->copy(i1 + 1, line->size());
		}

		if (request->version_ != "HTTP/1.1") throw UnsupportedVersion();

		String name, value;
		Ref<StringList> multiValue;
		while (source->read(&line)) {
			if (line == "")
				break;
			if (line->at(0) == ' ' || line->at(0) == '\t') {
				if (!multiValue) {
					multiValue = StringList::create();
					multiValue->append(value);
				}
				multiValue->append(line->trimInsitu());
				continue;
			}
			if (multiValue) {
				request->establish(name, multiValue->join());
				multiValue = 0;
			}
			int i = line->find(':');
			if (i == line->size()) throw BadRequest();
			name = line->copy(0, i)->trimInsitu();
			value = line->copy(i + 1, line->size())->trimInsitu();
			request->establish(name, value);
		}
		if (multiValue)
			request->establish(name, multiValue->join());
	}
	catch (ReadLimitExceeded &) {
		throw BadRequest();
	}

	request->host_ = request->value("Host");
	if (request->host_ == "") throw BadRequest();

	if (request->value("Transfer-Encoding") == "chunked") {
		requestStream_->nextChunk();
	}
	else {
		int64_t length = 0;
		String h;
		if (request->lookup("Content-Length", &h)) {
			bool ok = true;
			length = h->toInt64(&ok);
			if (!ok || length < 0) throw BadRequest();
		}
		requestStream_->nextPayload(length);
	}

	return request;
}

} // namespace fnode
