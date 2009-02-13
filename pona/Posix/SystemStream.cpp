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

#include <errno.h>
#include <unistd.h>
#include "../SystemStream.hpp"

namespace pona
{

SystemStream::SystemStream(int fd)
	: fd_(fd)
{
	isatty_ = ::isatty(fd);
}

SystemStream::~SystemStream()
{
	if ((fd_ > 2) && (isOpen()))
		close();
}

int SystemStream::readAvail(void* buf, int bufCapa)
{
	int ret = 0;
	while (true)
	{
		ret = ::read(fd_, buf, bufCapa);
		if (ret == -1) {
			if (errno == EINTR) continue;
			if (isatty_) { ret = 0; break; }
			PONA_THROW(StreamIoException, posixError());
		}
		break;
	}
	return ret;
}

void SystemStream::write(const void* buf, int bufFill)
{
	const uint8_t* buf2 = static_cast<const uint8_t*>(buf);
	while (bufFill > 0)
	{
		int ret = ::write(fd_, buf2, bufFill);
		if (ret == -1) {
			if (errno == EINTR) continue;
			PONA_THROW(StreamIoException, posixError());
		}
		buf2 += ret;
		bufFill -= ret;
	}
}

bool SystemStream::isOpen() const
{
	return fd_ != -1;
}

void SystemStream::close()
{
	if ((::close(fd_) == -1) && (!isatty_))
		PONA_THROW(StreamSemanticException, posixError());
	fd_ = -1;
}

} // namespace pona

