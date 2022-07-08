/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/FindLastNode>
#include <cc/Token>

namespace cc::syntax {

struct FindLastNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry):
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long offsetLast = -1;
        for (; offset <= text.count(); ++offset) {
            long h = entry_.matchNext(text, offset, production);
            if (h != -1) offsetLast = h;
        }

        if (offsetLast == -1) transaction.rollback();

        return offsetLast;
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
            "FindLast{" << entry_.explain() << "}";
    }

    SyntaxNode entry_;
};

FindLastNode::FindLastNode(const SyntaxNode &entry):
    SyntaxNode{new State{entry}}
{}

SyntaxNode FindLastNode::entry() const
{
    return me().entry_;
}

const FindLastNode::State &FindLastNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
