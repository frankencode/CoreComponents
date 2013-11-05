/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYNTAXDEBUGFACTORY_H
#define FLUX_SYNTAXDEBUGFACTORY_H

#include "SyntaxNode.h"

namespace flux
{

namespace syntax
{

class DefinitionNode;

class DebugFactory: public Object
{
public:
	virtual Node *produce(Node *newNode, const char *nodeType) = 0;

protected:
	DefinitionNode *definition() const;

private:
	friend class DefinitionNode;
	DefinitionNode *definition_;
};

} // namespace syntax

typedef syntax::DebugFactory SyntaxDebugFactory;

} // namespace flux

#endif // FLUX_SYNTAXDEBUGFACTORY_H
