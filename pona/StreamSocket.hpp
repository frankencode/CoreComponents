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

class StreamSocket: public SystemStream
{
public:
	StreamSocket(Ref<SocketAddress> address, TimeStamp idleTimeout = 1);
	void listen(int backlog = 8);
	void connect();
	void giveUp();
	
protected:
	virtual void init() {}
	virtual void idle() {}
	virtual void serve(Ref<SocketAddress> address, Ref<SystemStream> stream) {};
	virtual void cleanup() {}
	
	Ref<SocketAddress> address() const { return address_; }
	
private:
	Ref<SocketAddress, Owner> address_;
	TimeStamp idleTimeout_;
	bool giveUp_;
};

} // namespace pona

#endif // PONA_STREAMSOCKET_HPP
