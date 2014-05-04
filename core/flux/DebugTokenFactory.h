/*
 * Copyright (C) 2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_DEBUGTOKENFACTORY_H
#define FLUX_DEBUGTOKENFACTORY_H

#include "TokenFactory.h"
#include "DebugToken.h"

namespace flux
{

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
