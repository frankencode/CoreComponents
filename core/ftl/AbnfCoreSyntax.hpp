 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_ABNFCORESYNTAX_HPP
#define FTL_ABNFCORESYNTAX_HPP

#include "SyntaxDefinition.hpp"

namespace ftl
{

class AbnfCoreSyntax: public SyntaxDefinition
{
public:
	inline static hook<AbnfCoreSyntax> create(SyntaxDebugFactory *debugFactory = 0) { return new AbnfCoreSyntax(debugFactory); }

protected:
	AbnfCoreSyntax(SyntaxDebugFactory *debugFactory = 0);
};

} // namespace ftl

#endif // FTL_ABNFCORESYNTAX_HPP
