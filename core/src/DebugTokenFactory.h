/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_DEBUGTOKENFACTORY_H
#define FLUX_DEBUGTOKENFACTORY_H

#include <flux/TokenFactory>
#include <flux/DebugToken>

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
