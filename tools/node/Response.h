/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_RESPONSE_H
#define FNODE_RESPONSE_H

#include <fkit/String.h>
#include <fkit/Map.h>
#include <fkit/Stream.h>

namespace fnode
{

using namespace fkit;

class ClientConnection;
class PayloadSink;
class ServiceWorker;

class Response: public Map<String, String>
{
public:
	static Ref<Response> create(ClientConnection *client);

	void status(int statusCode, String reasonPhrase = "");
	void header(String name, String value);
	void begin();
	void write(String bytes);
	Format chunk(String pattern);
	Format chunk();
	void end();

private:
	friend class ServiceWorker;

	Response(ClientConnection *client);

	void writeHeader();
	PayloadSink *payload();

	inline bool delivered() const { return headerWritten_; }
	inline int statusCode() const { return statusCode_; }

	Ref<ClientConnection> client_;
	bool headerWritten_;
	Ref<PayloadSink> payload_;
	int statusCode_;
	String reasonPhrase_;
};

} // namespace fnode

#endif // FNODE_RESPONSE_H
