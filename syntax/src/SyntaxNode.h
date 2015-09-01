/*
 * Copyright (C) 2007-2015 Frank Mertens.
 *
 * Use of this source is governed by a BSD-style license that can be
 * found in the LICENSE file.
 *
 */

#ifndef FLUXSYNTAX_SYNTAXNODE_H
#define FLUXSYNTAX_SYNTAXNODE_H

#include <flux/Tree>
#include <flux/ByteArray>
#include <flux/syntax/SyntaxState>
#include <flux/syntax/Token>

namespace flux {
namespace syntax {

class Node: public Tree<Node>
{
public:
    inline static void rollBack(Token *parentToken, Token *lastChildSaved) {
        if (parentToken) {
            while (parentToken->lastChild() != lastChildSaved)
                parentToken->lastChild()->unlink();
        }
    }

    virtual int matchNext(ByteArray *text, int i, Token *parentToken, State *state) const = 0;

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

#endif // FLUXSYNTAX_SYNTAXNODE_H
