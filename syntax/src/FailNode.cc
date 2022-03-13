/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/FailNode>

namespace cc::syntax {

struct FailNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return -1;
    }

    long matchLength() const override
    {
        return 0;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Fail{}";
    }
};

FailNode::FailNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
