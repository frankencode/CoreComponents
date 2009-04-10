/*
 * StreamSocket.cpp -- stream-oriented sockets
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <unistd.h> // close, select
#include <fcntl.h> // fcntl
#include <errno.h> // errno
#include "StreamSocket.hpp"

namespace pona
{

StreamSocket::StreamSocket(Ref<SocketAddress> address, TimeStamp idleTimeout)
	: SystemStream(::socket(address->family(), SOCK_STREAM, 0)),
	  address_(address),
	  idleTimeout_(idleTimeout),
	  giveUp_(false)
{
	if (fd_ == -1)
		PONA_SYSTEM_EXCEPTION;
}

void StreamSocket::listen(int backlog)
{
	if (::bind(fd_, address_->socketAddress(), address_->socketAddressLength()) == -1)
		PONA_SYSTEM_EXCEPTION;
	
	if (::listen(fd_, backlog) == -1)
		PONA_SYSTEM_EXCEPTION;
	
	init();
	
	while (!giveUp_)
	{
		while (!giveUp_) {
			if (readyRead(idleTimeout_)) break;
			idle();
		}
		
		Ref<SocketAddress, Owner> remoteAddress = new SocketAddress(address_->family());
		socklen_t len = remoteAddress->socketAddressLength();
		int asd = ::accept(fd_, remoteAddress->socketAddress(), &len);
		if (asd < 0)
			PONA_SYSTEM_EXCEPTION;
		
		serve(remoteAddress, new SystemStream(asd));
	}
	
	::close(fd_);
	fd_ = -1;
	
	cleanup();
}

void StreamSocket::connect()
{
	int flags = ::fcntl(fd_, F_GETFL, 0);
	if (flags == -1)
		PONA_SYSTEM_EXCEPTION;
	
	if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
		PONA_SYSTEM_EXCEPTION;
	
	int ret = ::connect(fd_, address_->socketAddress(), address_->socketAddressLength());
	
	init();
	
	if (ret == -1) {
		if (errno != EINPROGRESS)
			PONA_SYSTEM_EXCEPTION;
		
		while (!giveUp_) {
			if (readyReadOrWrite(idleTimeout_)) break;
			idle();
		}
		
		int error = 0;
		socklen_t len = sizeof(error);
		if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
			PONA_SYSTEM_EXCEPTION;
		
		if (error != 0) {
			errno = error;
			PONA_SYSTEM_EXCEPTION;
		}
		
		if (::fcntl(fd_, F_SETFL, flags) == -1)
			PONA_SYSTEM_EXCEPTION;
	}
	
	serve(address_, this);
	
	::close(fd_);
	fd_ = -1;
	
	cleanup();
}

void StreamSocket::giveUp()
{
	giveUp_ = true;
}

} // namespace pona
