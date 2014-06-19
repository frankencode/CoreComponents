/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUXABNF_ABNFSYNTAXDEFINITION_H
#define FLUXABNF_ABNFSYNTAXDEFINITION_H

#include <flux/SyntaxDefinition.h>

namespace flux {
namespace abnf {

class AbnfSyntaxDefinition: public SyntaxDefinition
{
public:
	static Ref<AbnfSyntaxDefinition> create(SyntaxDebugFactory *debugFactory = 0);

protected:
	AbnfSyntaxDefinition(SyntaxDebugFactory *debugFactory = 0);
};

}} // namespace flux::abnf

#endif // FLUXABNF_ABNFSYNTAXDEFINITION_H
