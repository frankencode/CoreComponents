 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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
	  definitionId_(definition->id())
{
	flags_ = Flags::create(numFlags);
	captures_ = Captures::create(numCaptures);
	hint_ = 0;
	hintOffset_ = -1;

	if (parent) parent->child_ = this;
	for (int i = 0; i < flags_->length(); ++i) flags_->set(i, false);
	for (int i = 0; i < captures_->length(); ++i) captures_->set(i, Range::create());
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
