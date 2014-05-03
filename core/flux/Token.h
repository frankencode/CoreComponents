/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_TOKEN_H
#define FLUX_TOKEN_H

#include "Tree.h"

namespace flux
{

class Token;

class TokenScreen: public virtual Object
{
public:
	virtual bool project(Token *token, int i0, int i1) = 0;
};

namespace syntax {
	class RuleNode;
	class KeywordNode;
}

class Token: public Tree<Token>
{
public:
	Token()
		: scope_(-1),
		  rule_(-1),
		  keyword_(-1),
		  i0_(0),
		  i1_(0)
	{}

	Token(const Token &b)
		: scope_(b.scope_),
		  rule_(b.rule_),
		  keyword_(b.keyword_),
		  i0_(b.i0_),
		  i1_(b.i1_)
	{}

	inline bool valid() const { return rule_ != -1; }
	inline int scope() const { return scope_; }
	inline int rule() const { return rule_; }
	inline int keyword() const { return keyword_; }

	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }
	inline int index() const { return i0_; }
	inline int size() const { return i1_ - i0_; }

	bool glow(TokenScreen *screen);
	static void meld(Token *root0, Token *root1);

	Token *at(int i) const;

private:
	friend class syntax::RuleNode;
	friend class syntax::KeywordNode;

	inline void init(int scope, int rule)
	{
		scope_ = scope;
		rule_ = rule;
	}

	inline void setRange(int i0, int i1)
	{
		i0_ = i0;
		i1_ = i1;
	}

	inline void setKeyword(int value) { keyword_ = value; }

	bool burn(int b0, int b1);

	int scope_;
	int rule_;
	int keyword_;
	int i0_;
	int i1_;
};

} // namespace flux

#endif // FLUX_TOKEN_H
