/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/SequenceNode>
#include <cc/Token>

namespace cc::syntax {

struct SequenceNode::State: public SyntaxNode::State
{
    State(std::initializer_list<SyntaxNode> nodes):
        nodes_{nodes}
    {}

    State(const Array<SyntaxNode> &nodes):
        nodes_{nodes}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        auto transaction = production.children().transaction();

        for (const SyntaxNode &entry: nodes_) {
            offset = entry.matchNext(text, offset, production);
            if (offset == -1) {
                transaction.rollback();
                break;
            }
        }

        return offset;
    }

    long matchLength() const override
    {
        long totalLength = 0;
        for (const SyntaxNode &entry: nodes_) {
            long length = entry.matchLength();
            if (length >= 0) totalLength += length;
            else {
                totalLength = -1;
                break;
            }
        }
        return totalLength;
    }

    void decycle() override { nodes_ = Array<SyntaxNode>{}; }

    List<String> explain() const override
    {
        List<String> parts;
        parts << "Sequence{";
        for (const SyntaxNode &entry: nodes_)
            parts << entry.explain() << (entry == nodes_.last() ? "}" : ",");
        return parts;
    }

    Array<SyntaxNode> nodes_;
};

SequenceNode::SequenceNode(std::initializer_list<SyntaxNode> nodes):
    SyntaxNode{new State{nodes}}
{}

SequenceNode::SequenceNode(const Array<SyntaxNode> &nodes):
    SyntaxNode{new State{nodes}}
{}

Array<SyntaxNode> SequenceNode::nodes() const
{
    return me().nodes_;
}

const SequenceNode::State &SequenceNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
