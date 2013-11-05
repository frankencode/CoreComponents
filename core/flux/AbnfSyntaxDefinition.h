/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_ABNFSYNTAXDEFINITION_H
#define FLUX_ABNFSYNTAXDEFINITION_H

#include "SyntaxDefinition.h"

namespace flux
{

class AbnfSyntaxDefinition: public SyntaxDefinition
{
protected:
	AbnfSyntaxDefinition(SyntaxDebugFactory *debugFactory = 0);
};

} // namespace flux

#endif // FLUX_ABNFSYNTAXDEFINITION_H
