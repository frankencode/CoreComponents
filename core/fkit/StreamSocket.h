/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_STREAMSOCKET_H
#define FKIT_STREAMSOCKET_H

#include "SystemStream.h"
#include "SocketAddress.h"

namespace fkit
{

class StreamSocket: public SystemStream
{
public:
	static Ref<StreamSocket> listen(SocketAddress *address);
	static Ref<StreamSocket> connect(SocketAddress *address);

	SocketAddress *address() const;

	bool readyAccept(double interval);
	Ref<StreamSocket> accept();
	bool established(double interval);
	void shutdown(int how = SHUT_RDWR);

	void setRecvTimeout(double interval);
	void setSendTimeout(double interval);

protected:
	StreamSocket(SocketAddress *address);
	StreamSocket(SocketAddress *address, int fdc);
	void bind();
	void listen(int backlog = 8);
	void connect();

	Ref<SocketAddress> address_;
	bool connected_;
};

} // namespace fkit

#endif // FKIT_STREAMSOCKET_H
