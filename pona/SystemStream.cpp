/*
 * SystemStream.cpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */

#include <errno.h>
#include <unistd.h> // read, write, select
#include <fcntl.h> // fcntl
#include "SystemStream.hpp"

namespace pona
{

SystemStream::SystemStream(int fd)
	: fd_(fd),
	  isattyCached_(false)
{}

SystemStream::~SystemStream()
{
	if ((fd_ > 2) && (isOpen()))
		if (!interactive())
			close();
}

int SystemStream::fd() const { return fd_; }

bool SystemStream::interactive() const {
	if (!isattyCached_) {
		isatty_ = ::isatty(fd_);
		isattyCached_ = true;
	}
	return isatty_;
}

bool SystemStream::isOpen() const { return fd_ != -1; }

void SystemStream::close()
{
	if (::close(fd_) == -1)
		PONA_THROW(StreamSemanticException, systemError());
	fd_ = -1;
}

bool SystemStream::readyRead(TimeStamp timeout)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd_, &set);
	struct timeval tv;
	tv.tv_sec = timeout.secondsPart();
	tv.tv_usec = timeout.nanoSecondsPart() / 1000;
	int ret = ::select(fd_ + 1, &set, 0, 0, &tv);
	if (ret == -1)
		PONA_THROW(StreamSemanticException, systemError());
	return (ret > 0);
}

bool SystemStream::readyReadOrWrite(TimeStamp timeout)
{
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_SET(fd_, &rset);
	wset = rset;
	struct timeval tv;
	tv.tv_sec = timeout.secondsPart();
	tv.tv_usec = timeout.nanoSecondsPart() / 1000;
	int ret = ::select(fd_ + 1, &rset, &wset, 0, &tv);
	if (ret == -1)
		PONA_THROW(StreamSemanticException, systemError());
	return (ret > 0);
}

int SystemStream::readAvail(void* buf, int bufCapa)
{
	int ret = 0;
	while (true) {
		ret = ::read(fd_, buf, bufCapa);
		if (ret == -1) {
			if (errno == EINTR) continue;
			if (interactive()) { ret = 0; break; }
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

void SystemStream::makePrivate()
{
	if (::fcntl(fd_, F_SETFD, FD_CLOEXEC) == -1)
		PONA_THROW(StreamSemanticException, systemError());
}

} // namespace pona

