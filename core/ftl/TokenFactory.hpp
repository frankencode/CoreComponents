 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

	inline Token *rootToken() const { return rootToken_; }

private:
	O<Token> rootToken_;
	O<Token> previous_;
};

} // namespace ftl

#endif // FTL_TOKENFACTORY_HPP
