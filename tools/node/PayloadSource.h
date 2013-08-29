/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_PAYLOADSOURCE_H
#define FNODE_PAYLOADSOURCE_H

#include <fkit/Stream.h>

namespace fnode
{

using namespace fkit;

class Request;

class PayloadSource: public Stream
{
public:
	static Ref<PayloadSource> open(Stream *stream, Request *request);

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

	bool isConsumed() const;
	void drain();

private:
	PayloadSource(Stream *client, Request *request);

	Ref<Stream> stream_;
	int64_t contentLength_;
	int64_t bytesLeft_;
};

} // namespace fnode

#endif // FNODE_PAYLOADSOURCE_H
