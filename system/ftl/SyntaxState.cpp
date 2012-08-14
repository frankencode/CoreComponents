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

State::State(Ref<DefinitionNode> definition, int numFlags, int numChars, int numStrings, Ref<State> parent)
	: definition_(definition),
	  definitionId_(definition->id()),
	  flags_(Array<bool>::newInstance(numFlags)),
	  chars_(ByteArray::newInstance(numChars)),
	  strings_(Array< Ref<ByteArray, Owner> >::newInstance(numStrings)),
	  hint_(0),
	  hintOffset_(-1)
{
	if (parent)
		parent->child_ = this;
}

Ref<ByteArray> State::string(const char* name)
{
	return string(definition_->stringIdByName(name));
}

} // namespace syntax

} // namespace ftl
