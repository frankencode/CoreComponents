/*
 * UriSyntax.hpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_URISYNTAX_HPP
#define FTL_URISYNTAX_HPP

#include "AbnfDefinition.hpp"

namespace ftl
{

class UriSyntax: public AbnfDefinition
{
public:
	UriSyntax(Ref<DebugFactory> debugFactory = 0);
};

} // namespace ftl

#endif // FTL_URISYNTAX_HPP
