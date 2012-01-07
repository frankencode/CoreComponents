/*
 * Pipe.hpp -- the simplest IPC media
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
