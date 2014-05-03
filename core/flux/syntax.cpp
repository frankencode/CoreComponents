/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#include "SyntaxDebugFactory.h"
#include "syntax.h"

namespace flux
{

namespace syntax
{

NODE DefinitionNode::KEYWORD(const char *keywords)
{
	Ref<KeywordMap> map = KeywordMap::create();
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

void DefinitionNode::LINK()
{
	while (unresolvedLinkHead_) {
		unresolvedLinkHead_->rule_ = ruleByName(unresolvedLinkHead_->ruleName_);
		unresolvedLinkHead_ = unresolvedLinkHead_->unresolvedNext_;
	}
	while (unresolvedKeywordHead_) {
		unresolvedKeywordHead_->keyword_ = keywordByName(unresolvedKeywordHead_->keywordName_);
		unresolvedKeywordHead_ = unresolvedKeywordHead_->unresolvedKeywordNext_;
	}
	if (!LinkNode::rule_) {
		if (!LinkNode::ruleName_)
			FLUX_DEBUG_ERROR("Missing entry rule declaration");
		LinkNode::rule_ = ruleByName(LinkNode::ruleName_);
	}
}

State *DefinitionNode::createState() const
{
	return new State(this, numFlags_, numCaptures_);
}

Ref<State> DefinitionNode::find(ByteArray *text, int i) const
{
	Ref<State> state = createState();
	while (text->has(i)) {
		int h = matchNext(text, i, 0, state);
		if (h == -1) state->rootToken_ = 0;
		else break;
		++i;
	}
	return state;
}

Ref<State> DefinitionNode::match(ByteArray *text, int i) const
{
	Ref<State> state = createState();
	int h = i < 0 ? 0 : i;
	h = matchNext(text, h, 0, state);
	if ( (h == -1) || (i < 0 && h < text->size()) ) state->rootToken_ = 0;
	return state;
}

const DefinitionNode *DefinitionNode::resolveScope(const char *&name) const
{
	const DefinitionNode *scope = this;
	int k = 0;
	const char *p0 = name;
	const char *p = p0;
	while (true) {
		char ch = *(p++);
		if (!ch) break;
		k = (ch == ':') ? k + 1 : 0;
		if (k == 2) {
			Ref<DefinitionNode> childScope;
			if (!scope->scopeByName_->lookup(p0, p - p0 - k, &childScope))
				FLUX_DEBUG_ERROR(Format("Undefined scope '%%'") << name);
			scope = childScope;
			p0 = p;
			k = 0;
		}
	}
	name = p0;
	return scope;
}

int DefinitionNode::syntaxError(ByteArray *text, int index, State *state) const
{
	FLUX_DEBUG_ERROR("Unhandled syntax error");
	return -1;
}

int DefinitionNode::errorCallBack(Object *self, ByteArray *text, int index, State *state)
{
	DefinitionNode *definition = cast<DefinitionNode>(self);
	return definition->syntaxError(text, index, state);
}

} // namespace syntax

} // namespace flux
