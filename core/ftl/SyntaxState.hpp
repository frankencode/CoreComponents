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

#include "generics.hpp"
#include "Array.hpp"
#include "ByteArray.hpp"

namespace ftl
{

namespace syntax
{

class DefinitionNode;

class State: public Instance
{
public:
	inline bool flag(int id) const { return flags_->at(id); }
	bool flag(const char *name) const;
	inline void setFlag(int id, bool value) { flags_->set(id, value); }

	inline Ref<Range> capture(int id) const { return captures_->at(id); }
	Ref<Range> capture(const char *name) const;
	inline void setCapture(int id, Ref<Range> capture) { captures_->set(id, capture); }

	inline const char *hint() const { return hint_; }
	inline void setHint(const char *text) { hint_ = text; }

	inline int hintOffset() const { return hintOffset_; }
	inline void setHintOffset(int index) { hintOffset_ = index; }

	inline int definitionId() const { return definitionId_; }

	inline Ref<State> child() const { return child_; }
	inline void setChild(Ref<State> state) { child_ = state; }

private:
	friend class syntax::DefinitionNode;

	State();
	State(Ref<DefinitionNode> definition, int numFlags, int numCaptures, Ref<State> parent = 0);

	Ref<DefinitionNode, Owner> definition_;
	int definitionId_;

	Ref<State, Owner> child_;

	typedef Array<bool> Flags;
	typedef Array< Ref<Range, Owner> > Captures;

	Ref<Flags, Owner> flags_;
	Ref<Captures, Owner> captures_;

	const char *hint_;
	int hintOffset_;
};

} // namespace syntax

typedef syntax::State SyntaxState;

} // namespace ftl

#endif // FTL_SYNTAXSTATE_HPP
