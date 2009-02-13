/****************************************************************************
**
** This file is part of libPONA - The Portable Network Abstractions Library.
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

#ifndef PONA_TCPDISPATCHER_HPP
#define PONA_TCPDISPATCHER_HPP

#include "InetAddress.hpp"
#include "TcpStream.hpp"

namespace pona
{

class TcpSocket: public Instance
{
public:
	TcpSocket(Ref<InetAddress> address, TimeStamp idleTimeout = TimeStamp(1, 0), int backlog = 8);
	int run();
	void close();
	
	static Ref<TcpStream, Owner> connect(Ref<InetAddress> remoteAddress);
	
protected:
	virtual void init() {}
	virtual void serve(Ref<InetAddress> address, Ref<TcpStream> stream) = 0;
	virtual void idle() {}
	virtual void cleanup() {}
	
	Ref<InetAddress> localAddress() const { return localAddress_; }
	
private:
	Ref<InetAddress, Owner> localAddress_;
	TimeStamp idleTimeout_;
	bool doClose_;
	int lsd_; // listening socket descriptor
};

} // namespace pona

#endif // PONA_TCPDISPATCHER_HPP
