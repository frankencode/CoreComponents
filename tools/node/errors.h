/*
 * Copyright (C) 2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXNODE_ERRORS_H
#define FLUXNODE_ERRORS_H

#include <flux/String.h>

namespace fluxnode
{

using namespace flux;

class CloseRequest {};

class ProtocolError
{
public:
	ProtocolError(int statusCode, String message)
		: statusCode_(statusCode),
		  message_(message)
	{}

	inline int statusCode() const { return statusCode_; }
	inline String message() const { return message_; }

private:
	int statusCode_;
	String message_;
};

class BadRequest: public ProtocolError
{
public:
	BadRequest(): ProtocolError(400, "Bad Request") {}
};

class Forbidden: public ProtocolError
{
public:
	Forbidden(): ProtocolError(403, "Forbidden") {}
};

class NotFound: public ProtocolError
{
public:
	NotFound(): ProtocolError(404, "Not Found") {}
};

class UnsupportedVersion: public ProtocolError
{
public:
	UnsupportedVersion(): ProtocolError(505, "HTTP Version not supported") {}
};

class NotImplemented: public ProtocolError
{
public:
	NotImplemented(): ProtocolError(501, "Not implemented") {}
};

} // namespace fluxnode

#endif // FLUXNODE_ERRORS_H
