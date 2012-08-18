/*
 * SyntaxDebugFactory.hpp -- syntax debug node factory
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
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
