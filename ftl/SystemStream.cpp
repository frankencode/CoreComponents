/*
 * SystemStream.cpp -- a stream associated with a file descriptor
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <errno.h>
#include <string.h>
#include <unistd.h> // read, write, select
#include <fcntl.h> // fcntl
#include "SystemStream.hpp"

namespace ftl
{

SystemStream::SystemStream(int fd)
	: fd_(fd),
	  isattyCached_(false),
	  continueOnInterrupt_(false)
{}

SystemStream::~SystemStream()
{
	if (isOpen())
		if (fd_ >= 3) // because of StandardStreams concurrency
			close();
}

int SystemStream::fd() const { return fd_; }

bool SystemStream::isTeletype() const
{
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
		FTL_THROW(StreamSemanticException, systemError());
	fd_ = -1;
}

bool SystemStream::readyRead(Time timeout)
{
	fd_set set;
	FD_ZERO(&set);
	FD_SET(fd_, &set);
	struct timeval tv;
	tv.tv_sec = timeout.sec();
	tv.tv_usec = timeout.nsec() / 1000;
	int ret = ::select(fd_ + 1, &set, 0, 0, &tv);
	if (ret == -1)
		FTL_THROW(StreamSemanticException, systemError());
	return (ret > 0);
}

bool SystemStream::readyReadOrWrite(Time timeout)
{
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_SET(fd_, &rset);
	wset = rset;
	struct timeval tv;
	tv.tv_sec = timeout.sec();
	tv.tv_usec = timeout.nsec() / 1000;
	int ret = ::select(fd_ + 1, &rset, &wset, 0, &tv);
	if (ret == -1)
		FTL_THROW(StreamSemanticException, systemError());
	return (ret > 0);
}

int SystemStream::readAvail(void* buf, int bufCapa)
{
	int ret = 0;
	while (true) {
		ret = ::read(fd_, buf, bufCapa);
		if (ret == -1) {
			if ((errno == EINTR) && (continueOnInterrupt_))
				continue;
			if (isTeletype()) { ret = 0; break; } // fancy HACK, needs review
			FTL_THROW(StreamIoException, systemError());
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
			if ((errno == EINTR) && (continueOnInterrupt_))
				continue;
			FTL_THROW(StreamIoException, systemError());
		}
		buf2 += ret;
		bufFill -= ret;
	}
}

void SystemStream::closeOnExec()
{
	if (::fcntl(fd_, F_SETFD, FD_CLOEXEC) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

bool SystemStream::continueOnInterrupt() const { return continueOnInterrupt_; }
void SystemStream::setContinueOnInterrupt(bool on) { continueOnInterrupt_ = on; }

} // namespace ftl
