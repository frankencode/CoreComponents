/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_PAYLOADSINK_H
#define FNODE_PAYLOADSINK_H

#include <fkit/Stream.h>

namespace fnode
{

using namespace fkit;

class PayloadSink: public Stream
{
public:
	static Ref<PayloadSink> open(Stream *stream);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts, const char *sep = "");

private:
	PayloadSink(Stream *client);
	~PayloadSink();
	Ref<Stream> stream_;
};

} // namespace fnode

#endif // FNODE_PAYLOADSINK_H
