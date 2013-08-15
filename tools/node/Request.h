/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_REQUEST_H
#define FNODE_REQUEST_H

#include <fkit/String.h>
#include <fkit/Map.h>

namespace fnode
{

using namespace fkit;

class ClientConnection;

class Request: public Map<String, String>
{
public:
	static Ref<Request> parse(ClientConnection *client, int maxHeaderSize = 65536);

	inline String method() const { return method_; }
	inline String target() const { return target_; }
	inline String version() const { return version_; }
	inline String host() const { return host_; }

private:
	Request(ClientConnection *client, int maxHeaderSize);

	String method_;
	String target_;
	String version_;
	String host_;
};

} // namespace fnode

#endif // FNODE_REQUEST_H
