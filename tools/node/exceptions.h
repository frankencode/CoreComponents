/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FNODE_EXCEPTIONS_H
#define FNODE_EXCEPTIONS_H

#include <fkit/String.h>

namespace fnode
{

using namespace fkit;

class ProtocolException
{
public:
	ProtocolException(int statusCode, String message)
		: statusCode_(statusCode),
		  message_(message)
	{}

	inline int statusCode() const { return statusCode_; }
	inline String message() const { return message_; }

private:
	int statusCode_;
	String message_;
};

class BadRequest: public ProtocolException
{
public:
	BadRequest(): ProtocolException(400, "Bad Request") {}
};

class UnsupportedVersion: public ProtocolException
{
public:
	UnsupportedVersion(): ProtocolException(505, "HTTP Version not supported") {}
};

class NotImplemented: public ProtocolException
{
public:
	NotImplemented(): ProtocolException(501, "Not implemented") {}
};

} // namespace fnode

#endif // FNODE_EXCEPTIONS_H
