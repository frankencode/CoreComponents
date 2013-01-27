 /*
  * Copyright (C) 2007-2013 Frank Mertens.
  *
  * This program is free software; you can redistribute it and/or
  * modify it under the terms of the GNU General Public License
  * as published by the Free Software Foundation; either version
  * 2 of the License, or (at your option) any later version.
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

	virtual Node *succ(Node *node) const { return null<Node>(); }
	virtual int matchLength() const { return -1; }

	inline Node *succ() const {
		return parent() ? parent()->succ(Node::self()) : null<Node>();
	}

	inline Node *self() const { return const_cast<Node *>(this); }
};

typedef O<Node> NODE;

} // namespace syntax

typedef syntax::Node SyntaxNode;

} // namespace ftl

#endif // FTL_SYNTAXNODE_HPP
