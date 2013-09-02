/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "SystemLogStream.h"

namespace fnode
{

Ref<SystemLogStream> SystemLogStream::open(int priority)
{
	return new SystemLogStream(priority);
}

SystemLogStream::SystemLogStream(int priority)
	: priority_(priority)
{}

int SystemLogStream::read(ByteArray *buf)
{
	return 0;
}

void SystemLogStream::write(const ByteArray *buf)
{
	syslog(priority_, "%s", buf->constData());
}

void SystemLogStream::write(const StringList *parts)
{
	write(parts->join());
}

} // namespace fnode
