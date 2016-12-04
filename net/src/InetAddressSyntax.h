/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>

namespace cc { template<class> class Singleton; }

namespace cc {
namespace net {

/** \internal
  * \class InetAddressSyntax InetAddressSyntax.h cc/net/InetAddressSyntax
  * \brief Syntax of an internet address (IPv4 or IPv6)
  */
class InetAddressSyntax: public syntax::SyntaxDefinition
{
public:
    static const InetAddressSyntax *instance();

    inline int inet4Address() const { return inet4Address_; }
    inline int inet6Address() const { return inet6Address_; }
    inline int inetAddress() const { return inetAddress_; }

protected:
    friend class Singleton<InetAddressSyntax>;

    InetAddressSyntax(syntax::SyntaxDebugFactory *debugFactory = 0);

    int inet4Address_;
    int inet6Address_;
    int inetAddress_;
};

}} // namespace cc::net
