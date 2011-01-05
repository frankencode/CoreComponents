/*
 * StreamSocket.cpp -- stream-oriented sockets
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <unistd.h> // close, select
#include <fcntl.h> // fcntl
#include <errno.h> // errno
#include "StreamSocket.hpp"

namespace ftl
{

StreamSocket::StreamSocket(Ref<SocketAddress> address, int fd)
	: SystemStream(fd),
	  address_(address),
	  connected_(fd != -1)
{
	if (fd_ == -1) {
		fd_ = ::socket(address->family(), SOCK_STREAM, 0);
		if (fd_ == -1)
			FTL_THROW(StreamSemanticException, systemError());
	}
}

Ref<SocketAddress> StreamSocket::address() const { return address_; }

void StreamSocket::bind()
{
	if (::bind(fd_, address_->addr(), address_->addrLen()) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

void StreamSocket::listen(int backlog)
{
	if (::listen(fd_, backlog) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

bool StreamSocket::readyAccept(Time idleTimeout)
{
	return readyRead(idleTimeout);
}

Ref<StreamSocket, Owner> StreamSocket::accept()
{
	Ref<SocketAddress, Owner> clientAddress = new SocketAddress(address_->family());
	socklen_t len = clientAddress->addrLen();
	int fdc = ::accept(fd_, clientAddress->addr(), &len);
	if (fdc < 0)
		FTL_THROW(StreamSemanticException, systemError());
	return new StreamSocket(clientAddress, fdc);
}

void StreamSocket::connect()
{
	int flags = ::fcntl(fd_, F_GETFL, 0);
	if (flags == -1)
		FTL_THROW(StreamSemanticException, systemError());
	
	if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
		FTL_THROW(StreamSemanticException, systemError());
	
	int ret = ::connect(fd_, address_->addr(), address_->addrLen());
	
	if (ret == -1) {
		if (errno != EINPROGRESS)
			FTL_THROW(StreamSemanticException, systemError());
	}
	
	connected_ = (ret != -1);
	
	if (::fcntl(fd_, F_SETFL, flags) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

bool StreamSocket::established(Time idleTimeout)
{
	if (!connected_)
	{
		if (readyReadOrWrite(idleTimeout))
		{
			int error = 0;
			socklen_t len = sizeof(error);
			if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
				FTL_THROW(StreamSemanticException, systemError());
			
			if (error != 0) {
				errno = error;
				FTL_THROW(StreamSemanticException, systemError());
			}
			
			connected_ = true;
		}
	}
	
	return connected_;
}

void StreamSocket::shutdown(int how)
{
	::shutdown(fd_, how);
}

void StreamSocket::setRecvTimeout(Time idleTimeout)
{
	struct timeval tval;
	tval.tv_sec = idleTimeout.sec();
	tval.tv_usec = idleTimeout.usec();
	if (setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

void StreamSocket::setSendTimeout(Time idleTimeout)
{
	struct timeval tval;
	tval.tv_sec = idleTimeout.sec();
	tval.tv_usec = idleTimeout.usec();
	if (setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(tval)) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

Ref<SocketAddress> StreamSocket::localAddress() const { return localAddress(fd_); }
Ref<SocketAddress> StreamSocket::remoteAddress() const { return remoteAddress(fd_); }

Ref<SocketAddress> StreamSocket::localAddress(int fd)
{
	Ref<SocketAddress> address = new SocketAddress;
	socklen_t len = address->addrLen();
	if (::getsockname(fd, address->addr(), &len) == -1)
		FTL_THROW(StreamSemanticException, systemError());
	return address;
}

Ref<SocketAddress> StreamSocket::remoteAddress(int fd)
{
	Ref<SocketAddress> address = new SocketAddress;
	socklen_t len = address->addrLen();
	if (::getpeername(fd, address->addr(), &len) == -1)
		FTL_THROW(StreamSemanticException, systemError());
	return address;
}

} // namespace ftl
