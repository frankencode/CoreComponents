/*
 * Copyright (C) 2007-2013 Frank Mertens.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 */

#ifndef FLUX_SYNTAXNODE_H
#define FLUX_SYNTAXNODE_H

#include "Tree.h"
#include "SyntaxState.h"
#include "Token.h"
#include "TokenFactory.h"
#include "ByteArray.h"

namespace flux
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

	virtual int matchNext(ByteArray *text, int i, TokenFactory *tokenFactory, Token *parentToken, State *state) const = 0;

	virtual Node *succ(Node *node) const { return null<Node>(); }
	virtual int matchLength() const { return -1; }

	inline Node *succ() const {
		return parent() ? parent()->succ(Node::self()) : null<Node>();
	}

	inline Node *self() const { return const_cast<Node *>(this); }
};

typedef Ref<Node> NODE;

} // namespace syntax

typedef syntax::Node SyntaxNode;

} // namespace flux

#endif // FLUX_SYNTAXNODE_H
