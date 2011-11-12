/*
 * WireSyntax.hpp -- syntax of the Wire language
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_WIRESYNTAX_HPP
#define FTL_WIRESYNTAX_HPP

#include "Syntax.hpp"
#include "Array.hpp"

namespace ftl
{

class WireSyntax: public Syntax<ByteArray>::Definition
{
public:
	WireSyntax();
};

} // namespace ftl

#endif // FTL_WIRESYNTAX_HPP
