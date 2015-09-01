/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_DEBUGTOKENFACTORY_H
#define FLUXSYNTAX_DEBUGTOKENFACTORY_H

#include <flux/syntax/TokenFactory>
#include <flux/syntax/DebugToken>

namespace flux {

class DebugTokenFactory: public TokenFactory
{
public:
    inline static Ref<DebugTokenFactory> create() { return new DebugTokenFactory; }

protected:
    DebugTokenFactory() {}

    virtual Token *produce(int scope, int rule, const char *scopeName, const char *ruleName) {
        return new DebugToken(scope, rule, scopeName, ruleName);
    }
};

} // namespace flux

#endif // FLUX_DEBUGTOKENFACTORY_H
