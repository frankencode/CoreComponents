/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TOKENFACTORY_H
#define FLUX_TOKENFACTORY_H

#include "Token.h"

namespace flux
{

class SyntaxState;

class TokenFactory: public Object
{
	friend class SyntaxState;

public:
	inline static Ref<TokenFactory> create() { return new TokenFactory; }

protected:
	TokenFactory() {}
	virtual Token *produce(const char *ruleName) { return new Token; }

private:
	Ref<Token> previous_;
};

} // namespace flux

#endif // FLUX_TOKENFACTORY_H
