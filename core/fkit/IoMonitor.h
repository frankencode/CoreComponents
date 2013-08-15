/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_IOMONITOR_H
#define FKIT_IOMONITOR_H

#include "IoSet.h"

namespace fkit
{

class IoMonitor: public Object
{
public:
	inline static Ref<IoMonitor> create() { return new IoMonitor; }

	IoSet *readyRead();
	IoSet *readyWrite();
	IoSet *readyExcept();

	inline IoSet *readyAccept() { return readyRead(); }

	int wait(double timeout = -1);
	void reset();

protected:
	IoMonitor() {}
	Ref<IoSet> readyRead_;
	Ref<IoSet> readyWrite_;
	Ref<IoSet> readyExcept_;
};

} // namespace fkit

#endif // FKIT_IOMONITOR_H
