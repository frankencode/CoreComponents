/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_INETADDRESSSYNTAX_H
#define FLUXNET_INETADDRESSSYNTAX_H

#include <flux/syntax/SyntaxDefinition>

namespace flux { template<class> class Singleton; }

namespace flux {
namespace net {

/** \brief Syntax of an internet address (IPv4 or IPv6)
  */
class InetAddressSyntax: public syntax::SyntaxDefinition
{
public:
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

const InetAddressSyntax *inetAddressSyntax();

}} // namespace flux::net

#endif // FLUXNET_INETADDRESSSYNTAX_H
