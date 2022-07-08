/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/ContextNode>
#include <cc/Token>
#include <cc/str>

namespace cc::syntax {

struct ContextNode::State: public SyntaxNode::State
{
    State(const SyntaxRule *rule, const SyntaxNode &inside, const SyntaxNode &outside):
        rule_{rule},
        inside_{inside},
        outside_{outside}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        assert(rule_);
        return production.withinContext(*rule_) ?
            inside_.matchNext(text, offset, production) :
            outside_.matchNext(text, offset, production);
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Context{" << str(static_cast<const void *>(rule_)) << "," << inside_.explain() << "," << outside_.explain() << "}";
    }

    const SyntaxRule *rule_ { nullptr };
    SyntaxNode inside_;
    SyntaxNode outside_;
};

ContextNode::ContextNode(const SyntaxRule *context, const SyntaxNode &inside, const SyntaxNode &outside):
    SyntaxNode{new State{context, inside, outside}}
{}

} // namespace cc::syntax
