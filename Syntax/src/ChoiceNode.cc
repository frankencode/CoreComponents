/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/ChoiceNode>
#include <cc/Token>
#include <limits>

namespace cc::syntax {

struct ChoiceNode::State: public SyntaxNode::State
{
    State(std::initializer_list<SyntaxNode> choices):
        choices_{choices}
    {}

    State(const Array<SyntaxNode> &choices):
        choices_{choices}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long h = -1;
        for (const SyntaxNode &entry: choices_) {
            h = entry.matchNext(text, offset, production);
            if (h == -1) transaction.rollback();
            else break;
        }

        return h;
    }

    long matchLength() const override
    {
        long min = std::numeric_limits<long>::max(), max = -1;
        for (const SyntaxNode &entry: choices_) {
            long length = entry.matchLength();
            if (length < min) min = length;
            else if (max < length) max = length;
        }
        return (min != max) ? -1 : max;
    }

    void decycle() override { choices_ = Array<SyntaxNode>{}; }

    List<String> explain() const override
    {
        List<String> parts;
        parts << "Choice{";
        for (const SyntaxNode &choice: choices_)
            parts << choice.explain() << (choice == choices_.last() ? "}" : ",");
        return parts;
    }

    Array<SyntaxNode> choices_;
};

ChoiceNode::ChoiceNode(std::initializer_list<SyntaxNode> choices):
    SyntaxNode{new State{choices}}
{}

ChoiceNode::ChoiceNode(const Array<SyntaxNode> &choices):
    SyntaxNode{new State{choices}}
{}

Array<SyntaxNode> ChoiceNode::choices() const
{
    return me().choices_;
}

const ChoiceNode::State &ChoiceNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
