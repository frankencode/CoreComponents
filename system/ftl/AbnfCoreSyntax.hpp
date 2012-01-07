/*
 * AbnfCoreSyntax.hpp -- ABNF core syntax rules
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_ABNFCORESYNTAX_HPP
#define FTL_ABNFCORESYNTAX_HPP

#include "Syntax.hpp"
#include "Array.hpp"

namespace ftl
{

class AbnfCoreSyntax: public Syntax<ByteArray>::Definition
{
public:
	AbnfCoreSyntax(Ref<DebugFactory> debugFactory = 0);
};

} // namespace ftl

#endif // FTL_ABNFCORESYNTAX_HPP
