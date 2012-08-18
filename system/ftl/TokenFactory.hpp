/*
 * TokenFactory.hpp -- token allocation/initialization
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_TOKENFACTORY_HPP
#define FTL_TOKENFACTORY_HPP

#include <new>
#include "Token.hpp"

namespace ftl
{

class TokenFactory: public Instance
{
public:
	inline virtual Token *newToken() {
		Token *token = 0;
		if (previous_)
			if (previous_->refCount() == 1)
				token = previous_;
		if (!token) {
			token = new Token;
			previous_ = token;
		}
		return token;
	}

	inline Token *produce() {
		Token *token = newToken();
		if (!rootToken_) rootToken_ = token;
		return token;
	}

	inline Ref<Token> rootToken() const { return rootToken_; }

private:
	Ref<Token, Owner> rootToken_;
	Ref<Token, Owner> previous_;
};

} // namespace ftl

#endif // FTL_TOKENFACTORY_HPP
