/*
 * Token.hpp -- token of a token tree
 *
 * Copyright (c) 2007-2011, Frank Mertens
 *
 * See ../COPYING for the license.
 */
#ifndef FTL_TOKEN_HPP
#define FTL_TOKEN_HPP

#include "Tree.hpp"

namespace ftl
{

class TokenScreen;
class Token;

class Token: public Tree<Token>
{
public:
	Token()
		: definition_(-1),
		  rule_(-1),
		  keyword_(-1)
	{}
	
	Token(const Token& b)
		: definition_(b.definition_),
		  rule_(b.rule_),
		  keyword_(b.keyword_),
		  i0_(b.i0_),
		  i1_(b.i1_)
	{}
	
	inline void init(int definition, int rule)
	{
		definition_ = definition;
		rule_ = rule;
	}
	
	inline void setRange(int i0, int i1)
	{
		i0_ = i0;
		i1_ = i1;
	}
	
	inline int definition() const { return definition_; }
	inline int rule() const { return rule_; }
	inline int keyword() const { return keyword_; }
	inline void setKeyword(int value) { keyword_ = value; }
	
	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }
	inline int index() const { return i0_; }
	inline int length() const { return i1_ - i0_; }
	
	bool glow(Ref<TokenScreen> screen);
	static void meld(Ref<Token> root0, Ref<Token> root1);
	
	Ref<Token> at(int i) const;
	
private:
	bool burn(int b0, int b1);
	
	int definition_;
	int rule_;
	int keyword_;
	int i0_;
	int i1_;
};

} // namespace ftl

#endif // FTL_TOKEN_HPP
