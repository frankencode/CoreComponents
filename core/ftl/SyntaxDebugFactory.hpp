 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
  */
#ifndef FTL_SYNTAXDEBUGFACTORY_HPP
#define FTL_SYNTAXDEBUGFACTORY_HPP

#include "SyntaxNode.hpp"

namespace ftl
{

namespace syntax
{

class DefinitionNode;

class DebugFactory: public Instance
{
public:
	virtual Node *produce(Node *newNode, const char *nodeType) = 0;

protected:
	Ref<DefinitionNode> definition() const;

private:
	friend class DefinitionNode;
	Ref<DefinitionNode> definition_;
};

} // namespace syntax

typedef syntax::DebugFactory SyntaxDebugFactory;

} // namespace ftl

#endif // FTL_SYNTAXDEBUGFACTORY_HPP
