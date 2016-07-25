/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#pragma once

#include <cc/Tree>
#include <cc/ByteArray>
#include <cc/syntax/SyntaxState>
#include <cc/syntax/Token>

namespace cc {
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

}} // namespace cc::syntax

