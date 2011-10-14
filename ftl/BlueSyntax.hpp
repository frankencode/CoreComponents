/*
 * BlueSyntax.hpp -- Blue syntax
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_BLUESYNTAX_HPP
#define FTL_BLUESYNTAX_HPP

#include "Syntax.hpp"
#include "Array.hpp"

namespace ftl
{

class BlueSyntax: public Syntax<ByteArray>::Definition
{
public:
	BlueSyntax();
};

} // namespace ftl

#endif // FTL_BLUESYNTAX_HPP
