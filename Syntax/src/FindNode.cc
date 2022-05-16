/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/FindNode>
#include <cc/Token>

namespace cc::syntax {

struct FindNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry):
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        for (; offset <= text.count(); ++offset) {
            long h = entry_.matchNext(text, offset, production);
            if (h != -1) {
                offset = h;
                break;
            }
        }

        if (offset == -1) transaction.rollback();

        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{} <<
            "Find{" << entry_.explain() << "}";
    }

    SyntaxNode entry_;
};

FindNode::FindNode(const SyntaxNode &entry):
    SyntaxNode{new State{entry}}
{}

SyntaxNode FindNode::entry() const
{
    return me().entry_;
}

const FindNode::State &FindNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
