 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
