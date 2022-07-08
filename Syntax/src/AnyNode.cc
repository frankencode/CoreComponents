/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/AnyNode>

namespace cc::syntax {

struct AnyNode::State: public SyntaxNode::State
{
    long matchNext(const String &text, long offset, Token &production) const override
    {
        return text.has(offset) ? offset + 1 : -1;
    }

    long matchLength() const override
    {
        return 1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Any{}";
    }
};

AnyNode::AnyNode():
    SyntaxNode{new State}
{}

} // namespace cc::syntax
