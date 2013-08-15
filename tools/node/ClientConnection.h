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

#include <fkit/Channel.h>
#include <fkit/StreamSocket.h>
#include "Request.h"

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

	inline Stream *stream() const { return stream_; }
	inline SocketAddress *address() const { return address_; }
	inline String pendingData() const { return pendingData_; }
	inline Request *request() const { return request_; }

private:
	friend class ServiceWorker;
	friend class Request;

	ClientConnection(StreamSocket *socket)
		: stream_(socket),
		  address_(SocketAddress::getRemoteAddress(socket))
	{}

	Ref<Stream> stream_;
	Ref<SocketAddress> address_;
	String pendingData_;
	Ref<Request> request_;
};

} // namespace fnode

#endif // FNODE_CLIENTCONNECTION_H
