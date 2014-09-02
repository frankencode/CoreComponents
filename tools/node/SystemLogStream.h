/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_SYSTEMLOGSTREAM_H
#define FLUXNODE_SYSTEMLOGSTREAM_H

#include <syslog.h>
#include <flux/Stream.h>

namespace fluxnode
{

using namespace flux;

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

} // namespace fluxnode

#endif // FLUXNODE_SYSTEMLOGSTREAM_H
