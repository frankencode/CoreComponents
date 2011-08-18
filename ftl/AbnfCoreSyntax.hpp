/*
 * AbnfCoreSyntax.hpp -- ABNF core syntax rules
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
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
