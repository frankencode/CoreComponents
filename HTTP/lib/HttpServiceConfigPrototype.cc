/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/HttpServiceConfigPrototype>
#include <cc/HttpLoggingServiceRegistry>
#include <cc/TlsServerOptionsPrototype>

namespace cc {

HttpServiceConfigPrototype::HttpServiceConfigPrototype(const String &className, const MetaProtocol &protocol):
    MetaPrototype{className, protocol}
{
    establish("request-limit", 100);
    establish("request-payload-limit", 0x10000);
    establish("host", "");
    establish("uri", "");
    establish("security", TlsServerOptionsPrototype{}.as<MetaPrototype>());
    establish("error-log", HttpLoggingServiceRegistry{}.loggingProtocol());
    establish("access-log", HttpLoggingServiceRegistry{}.loggingProtocol());
}

} // namespace cc
