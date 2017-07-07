/*
 * Copyright (C) 2007-2017 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/syntax/Token>

namespace cc {
namespace syntax {

class SyntaxState;

class TokenFactory: public Object
{
    friend class SyntaxState;

public:
    inline static Ref<TokenFactory> create() { return new TokenFactory; }

protected:
    TokenFactory() {}
    virtual Token *produce(int scope, int rule, const char *scopeName, const char *ruleName) {
        return new Token(scope, rule);
    }
};

}} // namespace cc::syntax

