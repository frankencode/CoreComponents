/*
 * Token.hpp -- token of a token tree
 *
 * Copyright (c) 2007-2010, Frank Mertens
 *
 * See ../LICENSE for the license.
 */
#ifndef PONA_TOKEN_HPP
#define PONA_TOKEN_HPP

#include "Tree.hpp"

namespace pona
{

class TokenScreen;

class Token: public Tree
{
public:
	Token(): definition_(-1), rule_(-1), type_(-1) {}
	
	inline void init(int definition, int rule)
	{
		definition_ = definition;
		rule_ = rule;
		if (type_ == -1)
			type_ = rule;
	}
	
	inline void setRange(int i0, int i1)
	{
		i0_ = i0;
		i1_ = i1;
	}
	
	inline int definition() const { return definition_; }
	inline int rule() const { return rule_; }
	inline int type() const { return type_; }
	inline void setType(int value) { type_ = value; }
	
	inline int i0() const { return i0_; }
	inline int i1() const { return i1_; }
	inline int index() const { return i0_; }
	inline int length() const { return i1_ - i0_; }
	
	bool glow(Ref<TokenScreen> screen);
	
	static void meld(Ref<Token> root0, Ref<Token> root1);
	bool burn(int b0, int b1);
	
private:
	int definition_;
	int rule_;
	int type_;
	int i0_;
	int i1_;
};

} // namespace pona

#endif // PONA_TOKEN_HPP
