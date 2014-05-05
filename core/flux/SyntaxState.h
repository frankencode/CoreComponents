/*
 * Copyright (C) 2007-2014 Frank Mertens.
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
#include "Token.h"

namespace flux
{

namespace syntax
{

class DefinitionNode;
class RuleNode;
class InvokeNode;
class SetNode;
class IfNode;
class CaptureNode;
class ReplayNode;
class ExpectNode;
class ChoiceNode;
class GlueNode;

} // namespace syntax

class SyntaxDefinition;
class TokenFactory;

class SyntaxState: public Object
{
public:
	inline Token *rootToken() const { return rootToken_; }
	inline bool valid() const { return rootToken_; }
	inline int i0() const { return rootToken_ ? rootToken_->i0() : 0; }
	inline int i1() const { return rootToken_ ? rootToken_->i1() : -1; }

	bool flag(const char *name) const;
	Range *capture(const char *name) const;

	inline const char *hint() const { return hint_; }
	inline int hintOffset() const { return hintOffset_; }

private:
	friend class syntax::DefinitionNode;
	friend class syntax::RuleNode;
	friend class syntax::InvokeNode;
	friend class syntax::SetNode;
	friend class syntax::IfNode;
	friend class syntax::CaptureNode;
	friend class syntax::ReplayNode;
	friend class syntax::ExpectNode;
	friend class syntax::ChoiceNode;
	friend class syntax::GlueNode;

	typedef syntax::DefinitionNode DefinitionNode;

	SyntaxState();
	SyntaxState(const DefinitionNode *definition, int numFlags, int numCaptures, TokenFactory *tokenFactory);

	Token *produceToken(int scope, int rule, const char *scopeName, const char *ruleName);

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
	Ref<Token> rootToken_;

	Ref<TokenFactory> tokenFactory_;
	const char *hint_;
	int hintOffset_;
	bool finalize_;
};

namespace syntax { typedef SyntaxState State; }

} // namespace flux

#endif // FLUX_SYNTAXSTATE_H
