/*
 * WireSyntax.hpp -- syntax of the Wire language
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
