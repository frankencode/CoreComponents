/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_INETADDRESSSYNTAX_H
#define FLUX_INETADDRESSSYNTAX_H

#include "SyntaxDefinition.h"

namespace flux
{

template<class SubClass> class Singleton;

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

} // namespace flux

#endif // FLUX_INETADDRESSSYNTAX_H
