/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXABNF_ABNFSYNTAXDEFINITION_H
#define FLUXABNF_ABNFSYNTAXDEFINITION_H

#include <flux/SyntaxDefinition>

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
