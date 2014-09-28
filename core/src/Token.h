/*
 * Copyright (C) 2007-2014 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUX_TOKEN_H
#define FLUX_TOKEN_H

#include "Tree.h"

namespace flux
{

namespace syntax {
	class RuleNode;
	class InvokeNode;
	class KeywordNode;
}

class TokenFactory;
class Token;

class TokenScreen {
public:
	virtual bool project(Token *token, int i0, int i1) = 0;
};

class Token: public Tree<Token>
{
	friend class TokenFactory;
	friend class syntax::RuleNode;
	friend class syntax::InvokeNode;
	friend class syntax::KeywordNode;

public:
	inline bool valid() const { return rule_ != -1; }
	inline int scope() const { return scope_; }
	inline int rule() const { return rule_; }
	inline int keyword() const { return keyword_; }

	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }
	inline int index() const { return i0_; }
	inline int size() const { return i1_ - i0_; }

	bool project(TokenScreen *screen);

	Token *at(int i) const;

protected:
	Token(int scope = 0, int rule = 0)
		: scope_(scope),
		  rule_(rule),
		  keyword_(-1),
		  i0_(0),
		  i1_(0)
	{}

private:
	inline void setRange(int i0, int i1) {
		i0_ = i0;
		i1_ = i1;
	}

	inline void setKeyword(int value) { keyword_ = value; }

	int scope_;
	int rule_;
	int keyword_;
	int i0_;
	int i1_;
};

} // namespace flux

#endif // FLUX_TOKEN_H