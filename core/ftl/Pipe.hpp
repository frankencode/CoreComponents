 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

	inline static Ref<Pipe, Owner> create(int mode = Input) {
		return new Pipe(mode);
	}

	int childFd() const;
	void open();
	void childOpen();

private:
	Pipe(int mode);

	void open(int thisEnd);
	int mode_;
	int pipeFd_[2];
};

} // namespace ftl

#endif // FTL_PIPE_HPP
