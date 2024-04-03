/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TlsServerOptionsState>

namespace cc {

TlsServerOptions::TlsServerOptions(const MetaObject &config):
    Object{new State{config}}
{}

bool TlsServerOptions::hasCredentials() const
{
    return me().hasCredentials_;
}

double TlsServerOptions::sessionResumptionKeyRefresh() const
{
    return me().sessionResumptionKeyRefresh_;
}

TlsServerOptions::State &TlsServerOptions::me()
{
    return Object::me.as<State>();
}

const TlsServerOptions::State &TlsServerOptions::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
