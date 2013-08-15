/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FKIT_SYNTAXSTATE_H
#define FKIT_SYNTAXSTATE_H

#include "generics.h"
#include "Array.h"
#include "ByteArray.h"

namespace fkit
{

namespace syntax
{

class DefinitionNode;

class State: public Object
{
public:
	inline bool flag(int id) const { return flags_->at(id); }
	bool flag(const char *name) const;
	inline void setFlag(int id, bool value) { flags_->at(id) = value; }

	inline Range *capture(int id) const { return captures_->at(id); }
	Range *capture(const char *name) const;
	inline void setCapture(int id, Range *capture) { captures_->at(id) = capture; }

	inline const char *hint() const { return hint_; }
	inline void setHint(const char *text) { hint_ = text; }

	inline int hintOffset() const { return hintOffset_; }
	inline void setHintOffset(int index) { hintOffset_ = index; }

	inline int definitionId() const { return definitionId_; }

	inline State *child() const { return child_; }
	inline void setChild(State *state) { child_ = state; }

	inline bool finalize() const { return finalize_; }
	inline void setFinalize() { finalize_ = true; }

private:
	friend class syntax::DefinitionNode;

	State();
	State(const DefinitionNode *definition, int numFlags, int numCaptures, State *parent = 0);

	Ref<const DefinitionNode> definition_;
	int definitionId_;

	Ref<State> child_;

	typedef Array<bool> Flags;
	typedef Array< Ref<Range> > Captures;

	Ref<Flags> flags_;
	Ref<Captures> captures_;

	const char *hint_;
	int hintOffset_;

	bool finalize_;
};

} // namespace syntax

typedef syntax::State SyntaxState;

} // namespace fkit

#endif // FKIT_SYNTAXSTATE_H
