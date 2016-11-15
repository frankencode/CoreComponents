/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Exception>
#include <cc/net/SocketAddress>

namespace ccnode {

using namespace cc;
using namespace cc::net;

class CloseRequest {};

class ProtocolException: public Exception
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

class RequestTimeout: public ProtocolException
{
public:
    enum { StatusCode = 408 };
    RequestTimeout(): ProtocolException(StatusCode, "Request Timeout") {}
};

class PayloadTooLarge: public ProtocolException
{
public:
    enum { StatusCode = 413 };
    PayloadTooLarge(): ProtocolException(StatusCode, "Payload Too Large") {}
};

class InternalServerError: public ProtocolException
{
public:
    InternalServerError(): ProtocolException(500, "Internal Server Error") {}
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

class TlsError: public Exception
{
public:
    TlsError(int errorCode, const SocketAddress *peerAddress);
    String message() const;

private:
    int errorCode_;
    Ref<const SocketAddress> peerAddress_;
};

} // namespace ccnode
