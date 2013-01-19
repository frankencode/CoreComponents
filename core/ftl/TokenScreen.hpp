 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
