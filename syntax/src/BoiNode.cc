/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/BoiNode>

namespace cc::syntax {

struct BoiNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return (offset == 0) ? offset : -1;
    }

    long matchLength() const override
    {
        return 0;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Boi{}";
    }
};

BoiNode::BoiNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
