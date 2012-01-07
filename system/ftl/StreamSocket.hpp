/*
 * StreamSocket.hpp -- stream-oriented sockets
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_STREAMSOCKET_HPP
#define FTL_STREAMSOCKET_HPP

#include "atoms"
#include "defaults.hpp"
#include "Time.hpp"
#include "SystemStream.hpp"
#include "SocketAddress.hpp"

namespace ftl
{

class StreamSocket: public SystemStream
{
public:
	StreamSocket(Ref<SocketAddress> address, int fd = -1);
	Ref<SocketAddress> address() const;
	
	void bind();
	void listen(int backlog = FTL_DEFAULT_BACKLOG);
	bool readyAccept(Time idleTimeout);
	Ref<StreamSocket, Owner> accept();
	void connect();
	bool established(Time idleTimeout);
	void shutdown(int how = SHUT_RDWR);
	
	void setRecvTimeout(Time idleTimeout);
	void setSendTimeout(Time idleTimeout);
	
	Ref<SocketAddress> localAddress() const;
	Ref<SocketAddress> remoteAddress() const;
	static Ref<SocketAddress> localAddress(int fd);
	static Ref<SocketAddress> remoteAddress(int fd);
	
private:
	Ref<SocketAddress, Owner> address_;
	bool connected_;
};

} // namespace ftl

#endif // FTL_STREAMSOCKET_HPP
