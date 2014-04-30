/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYNTAXSTATE_H
#define FLUX_SYNTAXSTATE_H

#include "generics.h"
#include "Array.h"
#include "Map.h"

namespace flux
{

class SyntaxState;

namespace syntax
{

class DefinitionNode;
class SetNode;
class IfNode;
class CaptureNode;
class ReplayNode;
class ExpectNode;
class ChoiceNode;
class GlueNode;

} // namespace syntax

class SyntaxState: public Object
{
public:
	bool flag(const char *name) const;
	Range *capture(const char *name) const;

	inline const char *hint() const { return hint_; }
	inline int hintOffset() const { return hintOffset_; }

private:
	friend class syntax::DefinitionNode;
	friend class syntax::SetNode;
	friend class syntax::IfNode;
	friend class syntax::CaptureNode;
	friend class syntax::ReplayNode;
	friend class syntax::ExpectNode;
	friend class syntax::ChoiceNode;
	friend class syntax::GlueNode;

	typedef syntax::DefinitionNode DefinitionNode;

	SyntaxState();
	SyntaxState(const DefinitionNode *definition, int numFlags, int numCaptures);

	SyntaxState *stateByScope(const DefinitionNode *definition);

	inline bool flag(const DefinitionNode *scope, int flagId) {
		return stateByScope(scope)->flags_->at(flagId);
	}

	inline void setFlag(const DefinitionNode *scope, int flagId, bool value) {
		stateByScope(scope)->flags_->at(flagId) = value;
	}

	inline Range *capture(const DefinitionNode *scope, int captureId) {
		return stateByScope(scope)->captures_->at(captureId);
	}

	inline void setCapture(const DefinitionNode *scope, int captureId, Range *capture) {
		stateByScope(scope)->captures_->at(captureId) = capture;
	}

	Ref<const DefinitionNode> definition_;

	typedef Map< const DefinitionNode *, Ref<SyntaxState> > StateByScope;
	Ref<StateByScope> stateByScope_;

	typedef Array<bool> Flags;
	typedef Array< Ref<Range> > Captures;

	Ref<Flags> flags_;
	Ref<Captures> captures_;

	const char *hint_;
	int hintOffset_;
	bool finalize_;
};

namespace syntax { typedef SyntaxState State; }

} // namespace flux

#endif // FLUX_SYNTAXSTATE_H
