/*
 * SystemStream.cpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include <unistd.h>
#include "SystemStream.hpp"

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
			PONA_THROW(StreamIoException, systemError());
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
			PONA_THROW(StreamIoException, systemError());
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
		PONA_THROW(StreamSemanticException, systemError());
	fd_ = -1;
}

} // namespace pona

