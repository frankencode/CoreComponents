/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/LengthNode>
#include <cc/Token>
#include <cc/str>

namespace cc::syntax {

struct LengthNode::State: public SyntaxNode::State
{
    State(long minLength, long maxLength, const SyntaxNode &entry):
        minLength_{minLength},
        maxLength_{maxLength},
        entry_{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        long h = entry_.matchNext(text, offset, production);
        if (h != -1) {
            long d = h - offset;
            if ((d < minLength_) || (maxLength_ < d))
                h = -1;
        }

        if (h == -1) transaction.rollback();

        return h;
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    void decycle() override
    {
        entry_ = SyntaxNode{};
    }

    List<String> explain() const override
    {
        return List<String>{}
            << "Length{" << str(minLength_) << "," << str(maxLength_) << "," << entry_.explain() << "}";
    }

    long minLength_;
    long maxLength_;
    SyntaxNode entry_;
};

LengthNode::LengthNode(long minLength, long maxLength, const SyntaxNode &entry):
    SyntaxNode{new State{minLength, maxLength, entry}}
{}

/** \param minLength Required minimum length of the sub-expression match
  * \param entry Sub-expression to match
  */
LengthNode::LengthNode(long minLength, const SyntaxNode &entry):
    SyntaxNode{new State{minLength, std::numeric_limits<long>::max(), entry}}
{}

long LengthNode::minLength() const
{
    return me().minLength_;
}

long LengthNode::maxLength() const
{
    return me().maxLength_;
}

SyntaxNode LengthNode::entry() const
{
    return me().entry_;
}

const LengthNode::State &LengthNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
