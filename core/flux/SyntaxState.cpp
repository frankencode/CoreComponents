/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "syntax.h"
#include "SyntaxState.h"

namespace flux
{

namespace syntax
{

State::State()
	: hint_(0),
	  hintOffset_(-1),
	  finalize_(false)
{}

State::State(const DefinitionNode *definition, int numFlags, int numCaptures, State *parent)
	: definition_(definition),
	  definitionId_(definition->id()),
	  finalize_(false)
{
	flags_ = Flags::create(numFlags);
	captures_ = Captures::create(numCaptures);
	hint_ = 0;
	hintOffset_ = -1;

	if (parent) parent->child_ = this;
	for (int i = 0; i < flags_->size(); ++i) flags_->at(i) =  false;
	for (int i = 0; i < captures_->size(); ++i) captures_->at(i) = Range::create();
}

bool State::flag(const char *name) const
{
	return flag(definition_->flagIdByName(name));
}

Range *State::capture(const char *name) const
{
	return capture(definition_->captureIdByName(name));
}

} // namespace syntax

} // namespace flux
