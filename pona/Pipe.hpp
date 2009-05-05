/*
 * Pipe.hpp -- the simplest IPC media
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_PIPE_HPP
#define PONA_PIPE_HPP

#include "atom"
#include "SystemStream.hpp"

namespace pona
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

} // namespace pona

#endif // PONA_PIPE_HPP
