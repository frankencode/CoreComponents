/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_REQUESTSTREAM_H
#define FNODE_REQUESTSTREAM_H

#include <fkit/Stream.h>
#include "Request.h"

namespace fnode
{

using namespace fkit;

class RequestStream: public Stream
{
public:
	static Ref<RequestStream> open(Stream *stream);

	void setupTimeout(double interval);

	Ref<Request> readRequest();
	bool isConsumed() const;

	void putBack(Request *request);

private:
	RequestStream(Stream *stream);

	void nextHeader();
	void nextPayload(int64_t length);
	void nextLine();
	void nextChunk();

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

	Ref<Stream> stream_;
	Ref<Request> pendingRequest_;
	Ref<ByteArray> pending_;
	int64_t bytesLeft_;
	int nlCount_, nlMax_;
	bool eoi_;
	bool chunked_;
};

} // namespace fnode

#endif // FNODE_REQUESTSTREAM_H
