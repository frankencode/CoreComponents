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

class ChunkStream;

class Response: public Map<String, String>
{
public:
	static Ref<Response> create(Stream *client);

	void header(String name, String value);
	void begin();
	void write(String bytes);
	Format chunk(String pattern);
	Format chunk();
	void end();

private:
	Response(Stream *client);
	~Response();

	void writeHeader();

	Ref<Stream> client_;
	bool headerWritten_;
	Ref<ChunkStream> payload_;
};

} // namespace fnode

#endif // FNODE_RESPONSE_H
