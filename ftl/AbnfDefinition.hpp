/*
 * AbnfDefinition.hpp -- ABNF definitions
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef FTL_ABNFDEFINITION_HPP
#define FTL_ABNFDEFINITION_HPP

#include "Syntax.hpp"
#include "Array.hpp"

namespace ftl
{

class AbnfDefinition: public Syntax<ByteArray>::Definition
{
public:
	AbnfDefinition(Ref<DebugFactory> debugFactory = 0);
};

} // namespace ftl

#endif // FTL_ABNFDEFINITION_HPP
