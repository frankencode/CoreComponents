/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_URISYNTAX_H
#define FLUX_URISYNTAX_H

#include "SyntaxDefinition.h"

namespace flux
{

template<class SubClass> class Singleton;

class UriSyntax: public SyntaxDefinition
{
public:
	inline int userInfo() const { return userInfo_; }
	inline int host() const { return host_; }
	inline int port() const { return port_; }

	inline int scheme() const { return scheme_; }
	inline int authority() const { return authority_; }
	inline int path() const { return path_; }
	inline int query() const { return query_; }
	inline int fragment() const { return fragment_; }

	inline int uri() const { return uri_; }

protected:
	friend class Singleton<UriSyntax>;

	UriSyntax(SyntaxDebugFactory *debugFactory = 0);

	int userInfo_;
	int host_;
	int port_;
	int scheme_;
	int authority_;
	int path_;
	int query_;
	int fragment_;
	int uri_;
};

UriSyntax *uriSyntax();

} // namespace flux

#endif // FLUX_URISYNTAX_H
