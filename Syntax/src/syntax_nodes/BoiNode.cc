/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/syntax_node/BoiNode>

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
