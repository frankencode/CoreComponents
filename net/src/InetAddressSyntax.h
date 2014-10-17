/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXNET_INETADDRESSSYNTAX_H
#define FLUXNET_INETADDRESSSYNTAX_H

#include <flux/SyntaxDefinition>

namespace flux {

template<class SubClass> class Singleton;

namespace net {

class InetAddressSyntax: public SyntaxDefinition
{
public:
	inline int inet4Address() const { return inet4Address_; }
	inline int inet6Address() const { return inet6Address_; }
	inline int inetAddress() const { return inetAddress_; }

protected:
	friend class Singleton<InetAddressSyntax>;

	InetAddressSyntax(SyntaxDebugFactory *debugFactory = 0);

	int inet4Address_;
	int inet6Address_;
	int inetAddress_;
};

const InetAddressSyntax *inetAddressSyntax();

}} // namespace flux::net

#endif // FLUXNET_INETADDRESSSYNTAX_H
