/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_INETADDRESSSYNTAX_H
#define FKIT_INETADDRESSSYNTAX_H

#include "Singleton.h"
#include "SyntaxDefinition.h"

namespace fkit
{

class InetAddressSyntax: public SyntaxDefinition, public Singleton<InetAddressSyntax>
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

inline InetAddressSyntax *inetAddressSyntax() { return InetAddressSyntax::instance(); }

} // namespace fkit

#endif // FKIT_INETADDRESSSYNTAX_H
