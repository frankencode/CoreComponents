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
#include "stdio" // DEBUG
#include "StreamSocket.hpp"

namespace pona
{

StreamSocket::StreamSocket(Ref<SocketAddress> address, int fd)
	: SystemStream(fd),
	  address_(address),
	  connected_(fd != -1),
	  abort_(false)
{
	if (fd_ == -1) {
		fd_ = ::socket(address->family(), SOCK_STREAM, 0);
		if (fd_ == -1)
			PONA_SYSTEM_EXCEPTION;
	}
}

Ref<SocketAddress> StreamSocket::address() const { return address_; }

void StreamSocket::bind()
{
	if (::bind(fd_, address_->socketAddress(), address_->socketAddressLength()) == -1)
		PONA_SYSTEM_EXCEPTION;
}

void StreamSocket::listen(int backlog)
{
	if (::listen(fd_, backlog) == -1)
		PONA_SYSTEM_EXCEPTION;
}

bool StreamSocket::readyAccept(Time idleTimeout)
{
	return readyRead(idleTimeout);
}

Ref<StreamSocket, Owner> StreamSocket::accept()
{
	Ref<SocketAddress, Owner> clientAddress = new SocketAddress(address_->family());
	socklen_t len = clientAddress->socketAddressLength();
	int fdc = ::accept(fd_, clientAddress->socketAddress(), &len);
	if (fdc < 0)
		PONA_SYSTEM_EXCEPTION;
	
	return new StreamSocket(clientAddress, fdc);
}

void StreamSocket::connect()
{
	int flags = ::fcntl(fd_, F_GETFL, 0);
	if (flags == -1)
		PONA_SYSTEM_EXCEPTION;
	
	if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
		PONA_SYSTEM_EXCEPTION;
	
	int ret = ::connect(fd_, address_->socketAddress(), address_->socketAddressLength());
	
	if (ret == -1) {
		if (errno != EINPROGRESS)
			PONA_SYSTEM_EXCEPTION;
	}
	
	connected_ = (ret != -1);
	
	if (::fcntl(fd_, F_SETFL, flags) == -1)
		PONA_SYSTEM_EXCEPTION;
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
				PONA_SYSTEM_EXCEPTION;
			
			if (error != 0) {
				errno = error;
				PONA_SYSTEM_EXCEPTION;
			}
			
			connected_ = true;
		}
	}
	
	return connected_;
}

Ref<SocketAddress> StreamSocket::localAddress() const { return localAddress(fd_); }
Ref<SocketAddress> StreamSocket::remoteAddress() const { return remoteAddress(fd_); }

Ref<SocketAddress> StreamSocket::localAddress(int fd)
{
	Ref<SocketAddress> address = new SocketAddress;
	socklen_t len = address->socketAddressLength();
	if (::getsockname(fd, address->socketAddress(), &len) == -1)
		PONA_SYSTEM_EXCEPTION;
	return address;
}

Ref<SocketAddress> StreamSocket::remoteAddress(int fd)
{
	Ref<SocketAddress> address = new SocketAddress;
	socklen_t len = address->socketAddressLength();
	if (::getpeername(fd, address->socketAddress(), &len) == -1)
		PONA_SYSTEM_EXCEPTION;
	return address;
}

void StreamSocket::startServer(Time idleTimeout, int backlog)
{
	bind();
	listen(backlog);
	
	while (!abort_)
	{
		while (!abort_) {
			if (readyAccept(idleTimeout)) break;
			if (abort_) break;
			idle();
		}
		
		if (abort_) break;
		
		Ref<StreamSocket, Owner> socket = accept();
		if (!abort_)
			serve(socket);
	}
	
	close();
	cleanup();
}

void StreamSocket::startClient(Time idleTimeout)
{
	connect();
	while (!established(idleTimeout))
		idle();
	serve(this);
	close();
	cleanup();
}

void StreamSocket::abort() { abort_ = true; }
bool StreamSocket::stopServing() const { return abort_; }

} // namespace pona
