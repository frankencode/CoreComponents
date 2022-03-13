/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/RepeatNode>
#include <cc/Token>
#include <cc/str>

namespace cc::syntax {

struct RepeatNode::State: public SyntaxNode::State
{
    State(long minRepeat, long maxRepeat, const SyntaxNode &entry):
        minRepeat_{minRepeat},
        maxRepeat_{maxRepeat},
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long repeatCount = 0;
        for (; repeatCount < maxRepeat_; ++repeatCount) {
            long h = entry_.matchNext(text, offset, production);
            if (h == -1) break;
            if (h == offset) {
                transaction.rollback();
                return -1;
            }
            offset = h;
        }

        if (repeatCount < minRepeat_ || maxRepeat_ < repeatCount)
            offset = -1;

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
        return List<String>{}
            << "Repeat{" << str(minRepeat_) << "," << str(maxRepeat_) << "," << entry_.explain() << "}";
    }

    long minRepeat_;
    long maxRepeat_;
    SyntaxNode entry_;
};

RepeatNode::RepeatNode(long minRepeat, long maxRepeat, const SyntaxNode &entry):
    SyntaxNode{new State{minRepeat, maxRepeat, entry}}
{}

RepeatNode::RepeatNode(long minRepeat, const SyntaxNode &entry):
    SyntaxNode{new State{minRepeat, std::numeric_limits<long>::max(), entry}}
{}

RepeatNode::RepeatNode(const SyntaxNode &entry):
    SyntaxNode{new State{0, std::numeric_limits<long>::max(), entry}}
{}

long RepeatNode::minRepeat() const
{
    return me().minRepeat_;
}

long RepeatNode::maxRepeat() const
{
    return me().maxRepeat_;
}

SyntaxNode RepeatNode::entry() const
{
    return me().entry_;
}

const RepeatNode::State &RepeatNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
