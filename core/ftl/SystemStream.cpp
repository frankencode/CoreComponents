 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */

#include <sys/uio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h> // read, write, select
#include <fcntl.h> // fcntl
#include "SystemStream.hpp"

namespace ftl
{

SystemStream::SystemStream(int fd)
	: fd_(fd),
	  isattyCached_(false)
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

int SystemStream::readAvail(void *buf, int bufCapa)
{
	ssize_t ret = 0;
	while (true) {
		ret = ::read(fd_, buf, bufCapa);
		if (ret == -1) {
			if (errno == EINTR)
				throw Interrupt();
			if (errno == EWOULDBLOCK)
				throw Timeout();
			if (isTeletype()) { ret = 0; break; } // fancy HACK, needs review
			FTL_THROW(StreamIoException, systemError());
		}
		break;
	}
	return ret;
}

void SystemStream::write(const void *buf, int bufFill)
{
	const uint8_t *buf2 = static_cast<const uint8_t*>(buf);
	while (bufFill > 0)
	{
		ssize_t ret = ::write(fd_, buf2, bufFill);
		if (ret == -1) {
			if (errno == EINTR)
				throw Interrupt();
			if (errno == EWOULDBLOCK)
				throw Timeout();
			FTL_THROW(StreamIoException, systemError());
		}
		buf2 += ret;
		bufFill -= ret;
	}
}

void SystemStream::write(Ref<StringList> parts, const char *sep)
{
	int n = parts->length();
	int sepLen = str::len(sep);
	if (n <= 0) return;
	if (sepLen > 0) n += n - 1;
	struct iovec iov[n];
	for (int i = 0, j = 0; i < n; ++i) {
		if ((sepLen > 0) && ((i % 2) == 1)) {
			iov[i].iov_base = const_cast<char*>(sep);
			iov[i].iov_len = sepLen;
		}
		else {
			Ref<ByteArray> part = parts->at(j++);
			iov[i].iov_base = part->data();
			iov[i].iov_len = part->size();
		}
	}
	ssize_t ret = ::writev(fd_, &iov[0], n);
	if (ret == -1) {
		if (errno == EINTR)
			throw Interrupt();
		if (errno == EWOULDBLOCK)
			throw Timeout();
		FTL_THROW(StreamIoException, systemError());
	}
}

void SystemStream::closeOnExec()
{
	if (::fcntl(fd_, F_SETFD, FD_CLOEXEC) == -1)
		FTL_THROW(StreamSemanticException, systemError());
}

} // namespace ftl
