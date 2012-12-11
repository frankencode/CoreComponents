/*
 * UriSyntax.hpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_URISYNTAX_HPP
#define FTL_URISYNTAX_HPP

#include "AbnfCoreSyntax.hpp"
#include "Singleton.hpp"

namespace ftl
{

class UriSyntax: public AbnfCoreSyntax, public Singleton<UriSyntax>
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

	UriSyntax(Ref<SyntaxDebugFactory> debugFactory = 0);

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

inline Ref<UriSyntax> uriSyntax() { return UriSyntax::instance(); }

} // namespace ftl

#endif // FTL_URISYNTAX_HPP
