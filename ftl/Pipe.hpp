/*
 * Pipe.hpp -- the simplest IPC media
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_PIPE_HPP
#define FTL_PIPE_HPP

#include "atoms"
#include "SystemStream.hpp"

namespace ftl
{

class Pipe: public SystemStream
{
public:
	enum Mode { Input = 0, Output = 1 };
	
	Pipe(int mode = Input);
	int childFd() const;
	void open();
	void childOpen();
	
private:
	void open(int thisEnd);
	int mode_;
	int pipeFd_[2];
};

} // namespace ftl

#endif // FTL_PIPE_HPP
