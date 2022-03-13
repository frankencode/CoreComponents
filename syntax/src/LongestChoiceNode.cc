/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/LongestChoiceNode>
#include <cc/Token>
#include <limits>

namespace cc::syntax {

struct LongestChoiceNode::State: public SyntaxNode::State
{
    explicit State(std::initializer_list<SyntaxNode> choices):
        choices_{Array<SyntaxNode>::allocate(static_cast<long>(choices.size()))}
    {
        choices_ = choices;
    }

    explicit State(const Array<SyntaxNode> &choices):
        choices_{choices}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        SyntaxNode bestEntry;
        long m = -1;
        for (const SyntaxNode &entry: choices_) {
            long h = entry.matchNext(text, offset, production);
            transaction.rollback();
            if (m < h) {
                m = h;
                bestEntry = entry;
            }
        }

        if (bestEntry && !transaction.isEmpty())
            bestEntry.matchNext(text, offset, production);

        return m;
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
        parts << "LongestChoice{";
        for (const SyntaxNode &choice: choices_)
            parts << choice.explain() << (choice == choices_.last() ? "}" : ",");
        return parts;
    }

    Array<SyntaxNode> choices_;
};

LongestChoiceNode::LongestChoiceNode(std::initializer_list<SyntaxNode> choices):
    SyntaxNode{new State{choices}}
{}

LongestChoiceNode::LongestChoiceNode(const Array<SyntaxNode> &choices):
    SyntaxNode{new State{choices}}
{}

Array<SyntaxNode> LongestChoiceNode::choices() const
{
    return me().choices_;
}

const LongestChoiceNode::State &LongestChoiceNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
