/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include <unistd.h> // close, select
#include <fcntl.h> // fcntl
#include <errno.h> // errno
#include <math.h> // modf
#include "StreamSocket.h"

namespace fkit
{

Ref<StreamSocket> StreamSocket::listen(SocketAddress *address)
{
	Ref<StreamSocket> s = new StreamSocket(address);
	s->bind();
	s->listen();
	return s;
}

Ref<StreamSocket> StreamSocket::connect(SocketAddress *address)
{
	Ref<StreamSocket> s = new StreamSocket(address);
	s->connect();
	return s;
}

StreamSocket::StreamSocket(SocketAddress *address)
	: SystemStream(::socket(address->family(), SOCK_STREAM, 0)),
	  address_(address),
	  connected_(false)
{
	if (fd_ == -1)
		FKIT_SYSTEM_EXCEPTION;
}

StreamSocket::StreamSocket(SocketAddress *address, int fdc)
	: SystemStream(fdc),
	  address_(address),
	  connected_(true)
{}

SocketAddress *StreamSocket::address() const { return address_; }

void StreamSocket::bind()
{
	if (address_->port() != 0) {
		int on = 1;
		if (setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) == -1)
			FKIT_SYSTEM_EXCEPTION;
	}
	if (::bind(fd_, address_->addr(), address_->addrLen()) == -1)
		FKIT_SYSTEM_EXCEPTION;
}

void StreamSocket::listen(int backlog)
{
	if (::listen(fd_, backlog) == -1)
		FKIT_SYSTEM_EXCEPTION;
}

bool StreamSocket::readyAccept(double interval)
{
	return readyRead(interval);
}

Ref<StreamSocket> StreamSocket::accept()
{
	Ref<SocketAddress> clientAddress = SocketAddress::create(address_->family());
	socklen_t len = clientAddress->addrLen();
	int fdc = ::accept(fd_, clientAddress->addr(), &len);
	if (fdc < 0) {
		if (errno == EINTR) throw Interrupt();
		FKIT_SYSTEM_EXCEPTION;
	}
	return new StreamSocket(clientAddress, fdc);
}

void StreamSocket::connect()
{
	int flags = 0;

	if (address_->family() != AF_LOCAL) {
		flags = ::fcntl(fd_, F_GETFL, 0);
		if (flags == -1)
			FKIT_SYSTEM_EXCEPTION;
		if (::fcntl(fd_, F_SETFL, flags | O_NONBLOCK) == -1)
			FKIT_SYSTEM_EXCEPTION;
	}

	int ret = ::connect(fd_, address_->addr(), address_->addrLen());

	if (ret == -1) {
		if (errno != EINPROGRESS)
			FKIT_SYSTEM_EXCEPTION;
	}

	connected_ = (ret != -1);

	if (address_->family() != AF_LOCAL) {
		if (::fcntl(fd_, F_SETFL, flags) == -1)
			FKIT_SYSTEM_EXCEPTION;
	}
}

bool StreamSocket::established(double interval)
{
	if (!connected_)
	{
		if (readyReadOrWrite(interval))
		{
			int error = 0;
			socklen_t len = sizeof(error);
			if (::getsockopt(fd_, SOL_SOCKET, SO_ERROR, &error, &len) == -1)
				FKIT_SYSTEM_EXCEPTION;

			if (error != 0) {
				errno = error;
				FKIT_SYSTEM_EXCEPTION;
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

void StreamSocket::setRecvTimeout(double interval)
{
	struct timeval tval;
	double sec = 0;
	tval.tv_usec = modf(interval, &sec) * 1e6;
	tval.tv_sec = sec;
	if (::setsockopt(fd_, SOL_SOCKET, SO_RCVTIMEO, &tval, sizeof(tval)) == -1)
		FKIT_SYSTEM_EXCEPTION;
}

void StreamSocket::setSendTimeout(double interval)
{
	struct timeval tval;
	double sec = 0;
	tval.tv_usec = modf(interval, &sec) * 1e6;
	tval.tv_sec = sec;
	if (::setsockopt(fd_, SOL_SOCKET, SO_SNDTIMEO, &tval, sizeof(tval)) == -1)
		FKIT_SYSTEM_EXCEPTION;
}

} // namespace fkit
