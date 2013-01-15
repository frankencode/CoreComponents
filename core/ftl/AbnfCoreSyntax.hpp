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

#include "SyntaxDefinition.hpp"

namespace ftl
{

class AbnfCoreSyntax: public SyntaxDefinition
{
public:
	inline static Ref<AbnfCoreSyntax, Owner> create(Ref<SyntaxDebugFactory> debugFactory = 0) { return new AbnfCoreSyntax(debugFactory); }

protected:
	AbnfCoreSyntax(Ref<SyntaxDebugFactory> debugFactory = 0);
};

} // namespace ftl

#endif // FTL_ABNFCORESYNTAX_HPP
