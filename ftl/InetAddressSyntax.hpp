/*
 * InetAddressSyntax.hpp --  syntax of the Internet address literal
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_INETADDRESSSYNTAX_HPP
#define FTL_INETADDRESSSYNTAX_HPP

#include "Singleton.hpp"
#include "Syntax.hpp"

namespace ftl
{

class InetAddressSyntax: public Syntax<ByteArray>::Definition, public Singleton<InetAddressSyntax>
{
public:
	inline int inet4Address() const { return inet4Address_; }
	inline int inet6Address() const { return inet6Address_; }
	inline int inetAddress() const { return inetAddress_; }
	
private:
	InetAddressSyntax(Ref<DebugFactory> debugFactory = 0);
	
	friend class Singleton<InetAddressSyntax>;
	
	int inet4Address_;
	int inet6Address_;
	int inetAddress_;
};

inline Ref<InetAddressSyntax> inetAddressSyntax() { return InetAddressSyntax::instance(); }

} // namespace ftl

#endif // FTL_INETADDRESSSYNTAX_HPP
