/*
 * UriSyntax.hpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
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
	UriSyntax(Ref<DebugFactory> debugFactory = 0);
};

inline Ref<UriSyntax> uriSyntax() { return UriSyntax::instance(); }

} // namespace ftl

#endif // FTL_URISYNTAX_HPP
