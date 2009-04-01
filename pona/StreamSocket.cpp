/*
 * StreamSocket.cpp -- stream-oriented sockets
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // close, select
#include "StreamSocket.hpp"

namespace pona
{

StreamSocket::StreamSocket(Ref<SocketAddress> address, TimeStamp idleTimeout, int backlog)
	: localAddress_(address),
	  idleTimeout_(idleTimeout),
	  doClose_(false)
{
	lsd_ = ::socket(address->family(), SOCK_STREAM, 0);
	if (lsd_ == -1)
		PONA_SYSTEM_EXCEPTION;
	
	if (::bind(lsd_, address->socketAddress(), address->socketAddressLength()) == -1)
		PONA_SYSTEM_EXCEPTION;
	
	if (::listen(lsd_, backlog) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void StreamSocket::listen()
{
	init();
	
	while (true)
	{
		while (true) {
			fd_set set;
			FD_ZERO(&set);
			FD_SET(lsd_, &set);
			struct timeval timeout;
			timeout.tv_sec = idleTimeout_.secondsPart();
			timeout.tv_usec = idleTimeout_.nanoSecondsPart() / 1000;
			int ret = ::select(lsd_ + 1, &set, 0, 0, &timeout);
			if ((ret > 0) || doClose_) break;
			if (ret == -1) PONA_SYSTEM_EXCEPTION;
			idle();
		}
		
		if (doClose_) break;
		
		Ref<SocketAddress, Owner> remoteAddress = new SocketAddress(localAddress_->family());
		socklen_t len = remoteAddress->socketAddressLength();
		int asd = ::accept(lsd_, remoteAddress->socketAddress(), &len);
		if (asd < 0)
			PONA_SYSTEM_EXCEPTION;
			
		serve(remoteAddress, new SystemStream(asd));
	}
	
	::close(lsd_);
	
	cleanup();
}

void StreamSocket::close()
{
	doClose_ = true;
}

Ref<SystemStream, Owner> StreamSocket::connect(Ref<SocketAddress> remoteAddress)
{
	int sd = ::socket(remoteAddress->family(), SOCK_STREAM, 0);
	if (sd == -1)
		PONA_SYSTEM_EXCEPTION;
	if (::connect(sd, remoteAddress->socketAddress(), remoteAddress->socketAddressLength()) == -1)
		PONA_SYSTEM_EXCEPTION;
	return new SystemStream(sd);
}

} // namespace pona
