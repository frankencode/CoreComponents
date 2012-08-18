/*
 * SyntaxState.hpp -- state information of the syntax production engine
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SYNTAXSTATE_HPP
#define FTL_SYNTAXSTATE_HPP

#include "atoms"
#include "Array.hpp"
#include "ByteArray.hpp"

namespace ftl
{

namespace syntax {

class DefinitionNode;

class State: public Instance
{
public:
	inline int definitionId() const { return definitionId_; }

	inline bool *flag(int id) { return flags_->pointerAt(id); }
	inline char *character(int id) { return chars_->pointerAt(id); }

	inline Ref<ByteArray> string(int id) { return strings_->at(id); }
	inline void setString(int id, Ref<ByteArray> s) { strings_->set(id, s); }

	inline Ref<State> child() const { return child_; }
	inline void setChild(Ref<State> state) { child_ = state; }

	inline const char *hint() const { return hint_; }
	inline void setHint(const char *text) { hint_ = text; }

	inline int hintOffset() const { return hintOffset_; }
	inline void setHintOffset(int index) { hintOffset_ = index; }

	Ref<ByteArray> string(const char *name);

private:
	friend class syntax::DefinitionNode;
	State();
	State(Ref<DefinitionNode> definition, int numFlags, int numChars, int numStrings, Ref<State> parent = 0);

	Ref<DefinitionNode, Owner> definition_;
	int definitionId_;
	Ref< Array<bool>, Owner > flags_;
	Ref<ByteArray, Owner> chars_;
	Ref< Array< Ref<ByteArray, Owner> >, Owner > strings_;
	Ref<State, Owner> child_;
	const char *hint_;
	int hintOffset_;
};

} // namespace syntax

typedef syntax::State SyntaxState;

} // namespace ftl

#endif // FTL_SYNTAXSTATE_HPP
