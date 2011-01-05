/*
 * Pipe.cpp -- the simplest IPC media
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */

#include <unistd.h>
#include <fcntl.h>
#include "Pipe.hpp"

namespace ftl
{

Pipe::Pipe(int mode)
	: SystemStream(-1),
	  mode_(mode)
{
	if (::pipe(pipeFd_) == -1)
		FTL_SYSTEM_EXCEPTION;
	::fcntl(pipeFd_[mode_], F_SETFD, FD_CLOEXEC);
}

int Pipe::childFd() const { return pipeFd_[mode_ ^ 1]; }
void Pipe::open() { open(mode_); }
void Pipe::childOpen() { open(mode_ ^ 1); }

void Pipe::open(int thisEnd)
{
	int otherEnd = thisEnd ^ 1;
	if (fd_ == -1) {
		::close(pipeFd_[otherEnd]);
		pipeFd_[otherEnd] = -1;
		fd_ = pipeFd_[thisEnd];
	}
}

} // namespace ftl
