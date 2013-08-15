/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_ABNFSYNTAXDEFINITION_H
#define FKIT_ABNFSYNTAXDEFINITION_H

#include "SyntaxDefinition.h"

namespace fkit
{

class AbnfSyntaxDefinition: public SyntaxDefinition
{
protected:
	AbnfSyntaxDefinition(SyntaxDebugFactory *debugFactory = 0);
};

} // namespace fkit

#endif // FKIT_ABNFSYNTAXDEFINITION_H
