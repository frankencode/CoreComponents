/*
 * SyntaxState.cpp -- state information of the syntax production engine
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "syntax.hpp"
#include "SyntaxState.hpp"

namespace ftl
{

namespace syntax
{

State::State()
	: hint_(0),
	  hintOffset_(-1)
{}

State::State(Ref<DefinitionNode> definition, int numFlags, int numCaptures, Ref<State> parent)
	: definition_(definition),
	  definitionId_(definition->id()),
	  flags_(Flags::newInstance(numFlags)),
	  captures_(Captures::newInstance(numCaptures)),
	  hint_(0),
	  hintOffset_(-1)
{
	if (parent) parent->child_ = this;
	for (int i = 0; i < flags_->length(); ++i) flags_->set(i, false);
	for (int i = 0; i < captures_->length(); ++i) captures_->set(i, Range::newInstance());
}

bool State::flag(const char *name) const
{
	return flag(definition_->flagIdByName(name));
}

Ref<Range> State::capture(const char *name) const
{
	return capture(definition_->captureIdByName(name));
}

} // namespace syntax

} // namespace ftl
