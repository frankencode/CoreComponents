/*
 * StreamSocket.hpp -- stream-oriented sockets
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_STREAMSOCKET_HPP
#define PONA_STREAMSOCKET_HPP

#include "atoms"
#include "defaults.hpp"
#include "Time.hpp"
#include "Mutex.hpp"
#include "SystemStream.hpp"
#include "SocketAddress.hpp"

namespace pona
{

class StreamSocket: public SystemStream
{
	PONA_SHARED
	
public:
	StreamSocket(Ref<SocketAddress> address, int fd = -1);
	Ref<SocketAddress> address() const;
	
	void bind();
	void listen(int backlog = PONA_DEFAULT_BACKLOG);
	bool readyAccept(Time idleTimeout);
	Ref<StreamSocket, Owner> accept();
	void connect();
	bool established(Time idleTimeout);
	
	Ref<SocketAddress> localAddress() const;
	Ref<SocketAddress> remoteAddress() const;
	static Ref<SocketAddress> localAddress(int fd);
	static Ref<SocketAddress> remoteAddress(int fd);
	
	void startServer(Time idleTimeout = 0.1, int backlog = PONA_DEFAULT_BACKLOG);
	void startClient(Time idleTimeout = 0.1);
	void abort();
	
protected:
	virtual void idle() {}
	virtual void serve(Ref<StreamSocket> socket) {}
	virtual void cleanup() {}
	
	bool stopServing() const;
	
private:
	Ref<SocketAddress, Owner> address_;
	bool connected_;
	bool abort_;
};

} // namespace pona

#endif // PONA_STREAMSOCKET_HPP
