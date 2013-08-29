/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_SYSTEMLOGSTREAM_H
#define FNODE_SYSTEMLOGSTREAM_H

#include <syslog.h>
#include <fkit/Stream.h>

namespace fnode
{

using namespace fkit;

class SystemLogStream: public Stream
{
public:
	static Ref<SystemLogStream> open(int priority);

	virtual int read(ByteArray *buf);
	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	SystemLogStream(int priority);
	int priority_;
};

} // namespace fnode

#endif // FNODE_SYSTEMLOGSTREAM_H
