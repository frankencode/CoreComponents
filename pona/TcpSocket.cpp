/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstraction Library.
**
** Copyright (C) 2007-2009  Frank Mertens
**
** This file is part of a free software: you can redistribute it and/or
** modify it under the terms of the GNU General Public License as published
** by the Free Software Foundation, either version 3 of the License,
** or (at your option) any later version.
**
** The library is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this libary.  If not, see <http://www.gnu.org/licenses/>.
**
****************************************************************************/

#include <unistd.h> // close, select
#include "TcpSocket.hpp"

namespace pona
{

TcpSocket::TcpSocket(Ref<SocketAddress> address, TimeStamp idleTimeout, int backlog)
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

int TcpSocket::run()
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
			
		serve(remoteAddress, new TcpStream(asd));
	}
	
	::close(lsd_);
	
	cleanup();
	
	return 0;
}

void TcpSocket::close()
{
	doClose_ = true;
}

Ref<TcpStream, Owner> TcpSocket::connect(Ref<SocketAddress> remoteAddress)
{
	int sd = ::socket(remoteAddress->family(), SOCK_STREAM, 0);
	if (sd == -1)
		PONA_SYSTEM_EXCEPTION;
	if (::connect(sd, remoteAddress->socketAddress(), remoteAddress->socketAddressLength()) == -1)
		PONA_SYSTEM_EXCEPTION;
	return new TcpStream(sd);
}

} // namespace pona
