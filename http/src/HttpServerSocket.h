/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/http/HttpSocket>
#include <cc/http/SecuritySettings>

namespace cc {
namespace http {

using namespace cc::net;

class HttpServerSocket: public HttpSocket
{
public:
    static Ref<HttpServerSocket> connect(const SocketAddress *serverAddress, String serverName = "", SecuritySettings *security = 0, double timeout = -1);

private:
    HttpServerSocket(const SocketAddress *serverAddress, String serverName, SecuritySettings *security, double timeout);
    void connect();
    void initSession();
    void handshake();

    virtual bool waitInput() override;
    virtual void ioException(Exception &ex) const override;

    String serverName_;
    Ref<SecuritySettings> security_;
    double t0_;
    double te_;
};

}} // namespace cc::http
