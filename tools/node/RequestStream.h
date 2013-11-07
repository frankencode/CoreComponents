/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_REQUESTSTREAM_H
#define FLUXNODE_REQUESTSTREAM_H

#include <flux/Stream.h>
#include "Request.h"

namespace fluxnode
{

using namespace flux;

class RequestStream: public Stream
{
public:
	static Ref<RequestStream> open(Stream *stream);

	void setupTimeout(double interval);
	bool isPayloadConsumed() const;

	void nextHeader();
	void nextPayload(int64_t length);
	void nextLine();
	void nextChunk();

	virtual bool readyRead(double interval) const;
	virtual int read(ByteArray *buf);

	virtual void write(const ByteArray *buf);
	virtual void write(const StringList *parts);

private:
	RequestStream(Stream *stream);

	Ref<Stream> stream_;
	Ref<ByteArray> pending_;
	int64_t bytesLeft_;
	int nlCount_, nlMax_;
	bool eoi_;
	bool chunked_;
};

} // namespace fluxnode

#endif // FLUXNODE_REQUESTSTREAM_H
