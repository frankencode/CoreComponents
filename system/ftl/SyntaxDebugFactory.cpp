/*
 * SyntaxDebugFactory.cpp -- syntax debug node factory
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "syntax.hpp"
#include "SyntaxDebugFactory.hpp"

namespace ftl
{

namespace syntax
{

Ref<DefinitionNode> DebugFactory::definition() const { return definition_; }

} // namespace syntax

} // namespace ftl
