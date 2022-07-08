/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/RefNode>
#include <cc/SyntaxRule>
#include <cc/str>
#include <cassert>

namespace cc::syntax {

struct RefNode::State: public SyntaxNode::State
{
    State(const SyntaxRule *rule):
        rule_{rule}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        assert(rule_);
        return rule_->matchNext(text, offset, production);
    }

    long matchLength() const override
    {
        return rule_->matchLength();
    }

    List<String> explain() const override
    {
        return List<String>{} << "Ref{" << str(static_cast<const void *>(rule_)) << "}";
    }

    const SyntaxRule *rule_ { nullptr };
};

RefNode::RefNode(const SyntaxRule *rule):
    SyntaxNode{new State{rule}}
{}

} // namespace cc::syntax
