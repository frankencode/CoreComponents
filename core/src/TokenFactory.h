/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_TOKENFACTORY_H
#define FLUX_TOKENFACTORY_H

#include <flux/Token>

namespace flux {

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

} // namespace flux

#endif // FLUX_TOKENFACTORY_H
