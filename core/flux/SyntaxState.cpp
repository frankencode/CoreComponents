/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "syntax.h"
#include "Default.h"
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

State::State(const DefinitionNode *definition, int numFlags, int numCaptures)
	: definition_(definition),
	  flags_(Flags::create(numFlags)),
	  captures_(Captures::create(numCaptures)),
	  hint_(0),
	  hintOffset_(-1),
	  finalize_(false)
{
	for (int i = 0; i < flags_->size(); ++i) flags_->at(i) =  false;
	for (int i = 0; i < captures_->size(); ++i) captures_->at(i) = Range::create();
}

bool State::flag(const char *name) const
{
	const DefinitionNode *scope = definition_->resolveScope(name);
	int flagId = scope->flagIdByName(name);
	if (scope == definition_) return flags_->at(flagId);
	Ref<State> state;
	if (!stateByScope_->lookup(scope, &state)) return false;
	return state->flags_->at(flagId);
}

Range *State::capture(const char *name) const
{
	const DefinitionNode *scope = definition_->resolveScope(name);
	int captureId = scope->captureIdByName(name);
	if (scope == definition_) return captures_->at(captureId);
	Ref<State> state;
	if (!stateByScope_->lookup(scope, &state)) return 0;
	return state->captures_->at(captureId);
}

State *State::stateByScope(const DefinitionNode *scope)
{
	if (scope == definition_) return this;
	if (!stateByScope_) stateByScope_ = StateByScope::create();
	Ref<State> state;
	if (!stateByScope_->lookup(scope, &state)) {
		state = scope->newState();
		stateByScope_->insert(scope, state);
	}
	return state;
}

} // namespace syntax

} // namespace flux
