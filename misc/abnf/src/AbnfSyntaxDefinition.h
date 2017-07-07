/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/SyntaxDefinition>

namespace cc {
namespace abnf {

using namespace cc::syntax;

class AbnfSyntaxDefinition: public SyntaxDefinition
{
public:
    static Ref<AbnfSyntaxDefinition> create(SyntaxDebugFactory *debugFactory = 0);

protected:
    AbnfSyntaxDefinition(SyntaxDebugFactory *debugFactory = 0);
};

}} // namespace cc::abnf

