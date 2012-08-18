/*
 * SyntaxNode.hpp -- syntax tree node
 *
 * Copyright (c) 2007-2012, Frank Mertens
 *
 * This file is part of the a free software library. You can redistribute
 * it and/or modify it under the terms of FTL's 2-clause BSD license.
 *
 * See the LICENSE.txt file for details at the top-level of FTL's sources.
 */
#ifndef FTL_SYNTAXNODE_HPP
#define FTL_SYNTAXNODE_HPP

#include "Tree.hpp"
#include "SyntaxState.hpp"
#include "Token.hpp"
#include "TokenFactory.hpp"
#include "ByteArray.hpp"

namespace ftl
{

namespace syntax
{

class Node: public Tree<Node>
{
public:
	inline static void rollBack(Token *parentToken, Token *lastChildSaved) {
		if (parentToken) {
			while (parentToken->lastChild() != lastChildSaved)
				parentToken->lastChild()->unlink();
		}
	}

	virtual int matchNext(ByteArray *media, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const = 0;

	virtual Ref<Node> succ(Ref<Node> node) const { return Ref<Node>(); }
	virtual int matchLength() const { return -1; }

	inline Ref<Node> succ() const {
		return parent() ? parent()->succ(this) : Ref<Node>();
	}
};

typedef Ref<Node, Owner> NODE;

} // namespace syntax

typedef syntax::Node SyntaxNode;

} // namespace ftl

#endif // FTL_SYNTAXNODE_HPP
