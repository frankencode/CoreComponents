/*
 * TokenFactory.hpp -- token allocation/initialization
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TOKENFACTORY_HPP
#define PONA_TOKENFACTORY_HPP

#include <new>
#include "Token.hpp"

namespace pona
{

class TokenFactory: public Instance
{
public:
	inline virtual Token* newToken() {
		return new Token;
	}
	
	inline Token* produce()
	{
		Token* token = newToken();
		if (!rootToken_) rootToken_ = token;
		return token;
	}
	
	inline Ref<Token> rootToken() const { return rootToken_; }
	
private:
	Ref<Token, Owner> rootToken_;
};

} // namespace pona

#endif // PONA_TOKENFACTORY_HPP
