/*
 * syntax.cpp -- syntax description and production formalism
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */

#include "SyntaxDebugFactory.hpp"
#include "syntax.hpp"

namespace ftl
{

namespace syntax
{

int InvokeNode::matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const
{
	State *childState = 0;
	if (state) {
		childState = state->child();
		if (childState) {
			if (childState->definitionId() != definition_->id())
				childState = 0;
		}
		if (!childState)
			childState = definition_->newState(state);
	}

	if (coverage())
	{
		Ref<Token> lastChildSaved;
		if (parentToken) lastChildSaved = parentToken->lastChild();

		int i0 = i;
		i = coverage()->matchNext(media, i, 0, parentToken, state);

		if (i != -1)
		{
			rollBack(parentToken, lastChildSaved);

			Ref<ByteArray, Owner> range = ByteArray::create(media, i);
			definition_->matchNext(range, i0, tokenFactory, parentToken, childState);
		}
	}
	else {
		i = definition_->matchNext(media, i, tokenFactory, parentToken, childState);
	}

	return i;
}

NODE DefinitionNode::KEYWORD(const char *keywords)
{
	Ref<KeywordMap, Owner> map = KeywordMap::create();
	const char *pos = keywords;
	while (*pos) {
		if ((*pos == ' ') || (*pos == '\t')) {
			++pos;
			continue;
		}
		int len = 0;
		while (true) {
			char ch = *(pos + len);
			if ((ch == ' ') || (ch == '\t') || (ch == '\0')) break;
			++len;
		}
		int keyword = numKeywords_;
		numKeywords_ += keywordByName_->insert(pos, len, keyword, &keyword);
		map->insert(pos, len, keyword);
		pos += len;
	}
	return debug(new KeywordNode(map, caseSensitive_), "Keyword");
}

void DefinitionNode::OPTIMIZE()
{
	while (unresolvedLinkHead_) {
		Ref<LinkNode> link = unresolvedLinkHead_;
		link->rule_ = ruleByName(link->ruleName_);
		if (link->rule_->isVoid()) {
			if (Ref<RefNode>(link)) {
				if (link->rule_->numberOfRefs() == 0) {
					Ref<LinkNode, Owner> iLink = new InlineNode(link->ruleName_);
					link->parent()->insertChild(iLink, link);
					iLink->rule_ = link->rule_;
					link->unlink();
				}
			}
		}
		unresolvedLinkHead_ = link->unresolvedNext_;
	}
}

void DefinitionNode::LINK(bool optimize)
{
	if (optimize) OPTIMIZE();

	while (unresolvedLinkHead_) {
		unresolvedLinkHead_->rule_ = ruleByName(unresolvedLinkHead_->ruleName_);
		unresolvedLinkHead_ = unresolvedLinkHead_->unresolvedNext_;
	}
	while (unresolvedKeywordHead_) {
		unresolvedKeywordHead_->keyword_ = keywordByName(unresolvedKeywordHead_->keywordName_);
		unresolvedKeywordHead_ = unresolvedKeywordHead_->unresolvedKeywordNext_;
	}
	while (unresolvedInvokeHead_) {
		unresolvedInvokeHead_->definition_ = definitionByName(unresolvedInvokeHead_->definitionName_);
		unresolvedInvokeHead_ = unresolvedInvokeHead_->unresolvedNext_;
	}
	if (!LinkNode::rule_) {
		if (!LinkNode::ruleName_)
			FTL_THROW(DebugException, "Missing entry rule declaration");
		LinkNode::rule_ = ruleByName(LinkNode::ruleName_);
	}
}

State *DefinitionNode::newState(State *parent) const
{
	if (stateful())
		return new State(this, numFlags_, numCaptures_, parent);
	return 0;
}

Ref<Token, Owner> DefinitionNode::find(ByteArray *media, int *i0, int *i1, Ref<TokenFactory> tokenFactory) const
{
	int i = *i0;
	Ref<Token, Owner> rootToken;
	while (media->has(i)) {
		if (rootToken = match(media, i, i1, 0, tokenFactory))
			break;
		++i;
	}
	*i0 = i;
	return rootToken;
}

Ref<Token, Owner> DefinitionNode::match(ByteArray *media, int i0, int *i1, State *state, Ref<TokenFactory> tokenFactory) const
{
	Ref<State, Owner> localState;
	if (!state) {
		localState = newState();
		state = localState;
	}

	TokenFactory localTokenFactory;
	if (!tokenFactory)
		tokenFactory = &localTokenFactory;

	int h = matchNext(media, i0, tokenFactory, 0, state);

	if ((i1 != 0) && (h != -1))
		*i1 = h;

	return (h != -1) ? tokenFactory->rootToken() : Ref<Token>();
}

Ref<DefinitionNode> DefinitionNode::resolveScope(const char*& name) const
{
	Ref<DefinitionNode> scope = this;
	int k = 0;
	const char *p0 = name;
	const char *p = p0;
	while (true) {
		char ch = *(p++);
		if (!ch) break;
		k = (ch == ':') ? k + 1 : 0;
		if (k == 2) {
			Ref<DefinitionNode, Owner> childScope;
			if (!scope->definitionByName_->lookup(p0, p - p0 - k, &childScope))
				FTL_THROW(DebugException, str::cat("Undefined scope '", name, "' referenced"));
			scope = childScope;
			p0 = p;
			k = 0;
		}
	}
	name = p0;
	return scope;
}

int DefinitionNode::syntaxError(ByteArray *media, int index, State *state) const
{
	FTL_THROW(DebugException, "Unhandled syntax error");
	return -1;
}

int DefinitionNode::errorCallBack(Ref<Instance> self, ByteArray *media, int index, State *state)
{
	Ref<DefinitionNode> definition = self;
	return definition->syntaxError(media, index, state);
}

} // namespace syntax

} // namespace ftl
