/*
 * TokenScreen.hpp -- token tree projection target (see Token::burn())
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_TOKENSCREEN_HPP
#define FTL_TOKENSCREEN_HPP

#include "atoms"

namespace ftl
{

class Token;

class TokenScreen: public virtual Instance
{
public:
	virtual bool project(Ref<Token> token, int i0, int i1) = 0;
};

} // namespace ftl

#endif // FTL_TOKENSCREEN_HPP
