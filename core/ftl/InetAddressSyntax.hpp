/*
 * InetAddressSyntax.hpp --  syntax of the Internet address literal
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_INETADDRESSSYNTAX_HPP
#define FTL_INETADDRESSSYNTAX_HPP

#include "Singleton.hpp"
#include "SyntaxDefinition.hpp"

namespace ftl
{

class InetAddressSyntax: public SyntaxDefinition, public Singleton<InetAddressSyntax>
{
public:
	inline int inet4Address() const { return inet4Address_; }
	inline int inet6Address() const { return inet6Address_; }
	inline int inetAddress() const { return inetAddress_; }

protected:
	friend class Singleton<InetAddressSyntax>;

	InetAddressSyntax(Ref<SyntaxDebugFactory> debugFactory = 0);

	int inet4Address_;
	int inet6Address_;
	int inetAddress_;
};

inline Ref<InetAddressSyntax> inetAddressSyntax() { return InetAddressSyntax::instance(); }

} // namespace ftl

#endif // FTL_INETADDRESSSYNTAX_HPP
