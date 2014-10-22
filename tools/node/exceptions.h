/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNODE_EXCEPTIONS_H
#define FLUXNODE_EXCEPTIONS_H

#include <flux/String>

namespace fluxnode {

using namespace flux;

class CloseRequest {};

class ProtocolException
{
public:
    ProtocolException(int statusCode, String message):
        statusCode_(statusCode),
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

class Forbidden: public ProtocolException
{
public:
    Forbidden(): ProtocolException(403, "Forbidden") {}
};

class NotFound: public ProtocolException
{
public:
    NotFound(): ProtocolException(404, "Not Found") {}
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

} // namespace fluxnode

#endif // FLUXNODE_EXCEPTIONS_H
