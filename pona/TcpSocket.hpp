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

#ifndef PONA_TCPSOCKET_HPP
#define PONA_TCPSOCKET_HPP

#include "SocketAddress.hpp"
#include "TcpStream.hpp"

namespace pona
{

class TcpSocket: public Instance
{
public:
	TcpSocket(Ref<SocketAddress> address, TimeStamp idleTimeout = TimeStamp(1, 0), int backlog = 8);
	int run();
	void close();
	
	static Ref<TcpStream, Owner> connect(Ref<SocketAddress> remoteAddress);
	
protected:
	virtual void init() {}
	virtual void serve(Ref<SocketAddress> address, Ref<TcpStream> stream) = 0;
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

#endif // PONA_TCPSOCKET_HPP
