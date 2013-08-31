/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_CLIENTCONNECTION_H
#define FNODE_CLIENTCONNECTION_H

#include <fkit/StreamSocket.h>
#include "RequestStream.h"

namespace fnode
{

using namespace fkit;

class ServiceWorker;

class ClientConnection: public Object
{
public:
	inline static Ref<ClientConnection> create(StreamSocket *socket) {
		return new ClientConnection(socket);
	}

	inline RequestStream *stream() const { return stream_; }
	inline SocketAddress *address() const { return address_; }
	inline Request *request() const { return request_; }

	inline Ref<Request> readRequest() {
		request_ = 0;
		request_ = stream_->readRequest();
		return request_;
	}

private:
	friend class ServiceWorker;

	ClientConnection(StreamSocket *socket)
		: stream_(RequestStream::open(socket)),
		  address_(SocketAddress::getRemoteAddress(socket))
	{}

	Ref<RequestStream> stream_;
	Ref<SocketAddress> address_;
	Ref<Request> request_;
};

} // namespace fnode

#endif // FNODE_CLIENTCONNECTION_H
