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

class SyntaxNode: public Tree<SyntaxNode>
{
public:
    inline static void rollBack(Token *parentToken, Token *lastChildSaved) {
        if (parentToken) {
            while (parentToken->lastChild() != lastChildSaved)
                parentToken->lastChild()->unlink();
        }
    }

    virtual int matchNext(ByteArray *text, int i, Token *parentToken, SyntaxState *state) const = 0;

    virtual SyntaxNode *succ(SyntaxNode *node) const { return null<SyntaxNode>(); }
    virtual int matchLength() const { return -1; }

    inline SyntaxNode *succ() const {
        return parent() ? parent()->succ(SyntaxNode::self()) : null<SyntaxNode>();
    }

    inline SyntaxNode *self() const { return const_cast<SyntaxNode *>(this); }
};

typedef Ref<SyntaxNode> NODE;

}} // namespace flux::syntax

#endif // FLUXSYNTAX_SYNTAXNODE_H
