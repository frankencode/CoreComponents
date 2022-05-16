/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/TlsClientOptionsState>

namespace cc {

void TlsClientOptions::setServerName(const String &serverName)
{
    me().serverName_ = serverName;
}

void TlsClientOptions::setCredentials(const String &certPath, const String &keyPath)
{
    me().setCredentials(certPath, keyPath);
}

void TlsClientOptions::setTrustFilePath(const String &filePath)
{
    me().setTrustFilePath(filePath);
}

void TlsClientOptions::setCiphers(const String &ciphers)
{
    me().setCiphers(ciphers);
}

String TlsClientOptions::serverName() const
{
    return me().serverName_;
}

String TlsClientOptions::certPath() const
{
    return me().certPath_;
}

String TlsClientOptions::keyPath() const
{
    return me().keyPath_;
}

String TlsClientOptions::trustFilePath() const
{
    return me().trustFilePath_;
}

String TlsClientOptions::ciphers() const
{
    return me().ciphers_;
}

const TlsClientOptions::State &TlsClientOptions::me() const
{
    return Object::me.as<State>();
}

TlsClientOptions::State &TlsClientOptions::me()
{
    if (!Object::me) Object::me = new State;
    return Object::me.as<State>();
}

} // namespace cc
