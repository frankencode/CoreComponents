/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

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
{}

Ref<Request> ClientConnection::readRequest()
{
	if (pendingRequest_) {
		Ref<Request> h = pendingRequest_;
		pendingRequest_ = 0;
		return h;
	}
	request_ = 0;
	request_ = requestStream_->readRequest();
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

} // namespace fnode
