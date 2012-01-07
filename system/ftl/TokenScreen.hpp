/*
 * TokenScreen.hpp -- token tree projection target (see Token::burn())
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
