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
#include "DebugToken.h"

namespace flux
{

class TokenFactory: public Object
{
public:
	inline static Ref<TokenFactory> create() { return new TokenFactory; }

	inline Token *produce(const char *ruleName)
	{
		Token *token = 0;
		if (previous_)
			if (previous_->refCount() == 1)
				token = previous_;
		if (!token) {
			if (!debugging_)
				token = new Token;
			else
				token = new DebugToken(ruleName);
			previous_ = token;
		}
		return token;
	}

	inline bool debugging() const { return debugging_; }
	inline void setDebugging(bool on) { debugging_ = on; }

private:
	TokenFactory(): debugging_(false) {}
	bool debugging_;
	Ref<Token> previous_;
};

} // namespace flux

#endif // FLUX_TOKENFACTORY_H
