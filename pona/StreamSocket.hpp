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
#include "TimeStamp.hpp"
#include "SocketAddress.hpp"
#include "SystemStream.hpp"

namespace pona
{

class StreamSocket: public Instance
{
public:
	StreamSocket(Ref<SocketAddress> address, TimeStamp idleTimeout = 1, int backlog = 8);
	void listen();
	void close();
	
	static Ref<SystemStream, Owner> connect(Ref<SocketAddress> remoteAddress);
	
protected:
	virtual void init() {}
	virtual void serve(Ref<SocketAddress> address, Ref<SystemStream> stream) = 0;
	virtual void idle() {}
	virtual void cleanup() {}
	
	Ref<SocketAddress> localAddress() const { return localAddress_; }
	
private:
	Ref<SocketAddress, Owner> localAddress_;
	TimeStamp idleTimeout_;
	bool doClose_;
	int lsd_; // listening socket descriptor
};

} // namespace pona

#endif // PONA_STREAMSOCKET_HPP
