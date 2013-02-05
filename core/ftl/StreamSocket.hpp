 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_STREAMSOCKET_HPP
#define FTL_STREAMSOCKET_HPP

#include "defaults.hpp"
#include "Time.hpp"
#include "SystemStream.hpp"
#include "SocketAddress.hpp"

namespace ftl
{

class StreamSocket: public SystemStream
{
public:
	inline static hook<StreamSocket> create(SocketAddress *address, int fd = -1) {
		return new StreamSocket(address, fd);
	}

	SocketAddress *address() const;

	void bind();
	void listen(int backlog = FTL_DEFAULT_BACKLOG);
	bool readyAccept(Time idleTimeout);
	hook<StreamSocket> accept();
	void connect();
	bool established(Time idleTimeout);
	void shutdown(int how = SHUT_RDWR);

	void setRecvTimeout(Time idleTimeout);
	void setSendTimeout(Time idleTimeout);

	SocketAddress *localAddress() const;
	SocketAddress *remoteAddress() const;
	static SocketAddress *localAddress(int fd);
	static SocketAddress *remoteAddress(int fd);

protected:
	StreamSocket(SocketAddress *address, int fd);
	hook<SocketAddress> address_;
	bool connected_;
};

} // namespace ftl

#endif // FTL_STREAMSOCKET_HPP
