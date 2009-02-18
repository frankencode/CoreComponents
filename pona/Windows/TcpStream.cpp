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

#include <Winsock2.h>
#include "../Exception.hpp"
#include "../TcpStream.hpp"

namespace pona
{

TcpStream::TcpStream(int sd)
	: sd_(sd)
{}

TcpStream::~TcpStream()
{
	if (isOpen()) close();
}

bool TcpStream::isOpen() const
{
	return sd_ != 0;
}

void TcpStream::close()
{
	if (isOpen()) {
		::closesocket(sd_);
		sd_ = 0;
	}
}

int TcpStream::readAvail(void* buf, int bufCapa)
{
	int ret = ::recv(sd_, (char*)buf, bufCapa, 0);
	if (ret == SOCKET_ERROR)
		PONA_THROW(StreamIoException, windowsError());
	return ret;
}

void TcpStream::write(const void* buf, int bufFill)
{
	while (bufFill > 0)
	{
		int ret = ::send(sd_, (char*)buf, bufFill, 0);
		if (ret == SOCKET_ERROR)
			PONA_THROW(StreamIoException, windowsError());
		bufFill -= ret;
		buf = (uint8_t*)buf + ret;
	}
}

} // namespace pona
