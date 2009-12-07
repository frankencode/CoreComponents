/*
 * TokenScreen.hpp -- token tree projection target (see Token::burn())
 *
 * Copyright (c) 2007-2009, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TOKENSCREEN_HPP
#define PONA_TOKENSCREEN_HPP

#include "atoms"

namespace pona
{

class Token;

class TokenScreen: public virtual Instance
{
public:
	virtual bool project(Ref<Token> token, int i0, int i1) = 0;
};

} // namespace pona

#endif // PONA_TOKENSCREEN_HPP
