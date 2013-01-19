 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
