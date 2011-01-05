/*
 * UriSyntax.hpp -- URI syntax according to RFC3986
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_URISYNTAX_HPP
#define FTL_URISYNTAX_HPP

#include "Syntax.hpp"

namespace ftl
{

class UriSyntax: public Syntax<ByteArray>::Definition
{
public:
	UriSyntax();
};

} // namespace ftl

#endif // FTL_URISYNTAX_HPP
