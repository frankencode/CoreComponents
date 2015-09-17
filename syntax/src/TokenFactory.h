/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_TOKENFACTORY_H
#define FLUXSYNTAX_TOKENFACTORY_H

#include <flux/syntax/Token>

namespace flux {
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

}} // namespace flux::syntax

#endif // FLUXSYNTAX_TOKENFACTORY_H
