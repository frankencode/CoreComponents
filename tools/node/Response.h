/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_RESPONSE_H
#define FLUXNODE_RESPONSE_H

#include <flux/String.h>
#include <flux/Map.h>
#include <flux/Stream.h>

namespace flux {
namespace stream {
class TransferMeter;
}}

namespace fluxnode
{

using namespace flux;
using namespace flux::stream;

class ClientConnection;
class ServiceWorker;

class Response: public Map<String, String>
{
public:
	static Ref<Response> create(ClientConnection *client);

	void status(int statusCode, String reasonPhrase = "");
	void header(String name, String value);
	void begin(ssize_t contentLength = -1);
	void write(String bytes);
	Format chunk(String pattern);
	Format chunk();
	void end();

private:
	friend class ServiceWorker;

	Response(ClientConnection *client);

	void writeHeader();
	Stream *payload();

	inline bool delivered() const { return headerWritten_; }
	inline int statusCode() const { return statusCode_; }
	size_t bytesWritten() const;

	Ref<ClientConnection> client_;
	bool headerWritten_;
	Ref<TransferMeter> payload_;
	int statusCode_;
	ssize_t contentLength_;
	size_t bytesWritten_;
	String reasonPhrase_;
};

} // namespace fluxnode

#endif // FLUXNODE_RESPONSE_H
