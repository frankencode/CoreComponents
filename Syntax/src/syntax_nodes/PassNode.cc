/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/syntax_node/PassNode>

namespace cc::syntax {

struct PassNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Pass{}";
    }

};

PassNode::PassNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
