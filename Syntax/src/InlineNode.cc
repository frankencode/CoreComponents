/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/InlineNode>
#include <cc/SyntaxRule>
#include <cc/str>
#include <cassert>

namespace cc::syntax {

struct InlineNode::State: public SyntaxNode::State
{
    State(const SyntaxRule *rule):
        rule_{rule}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        assert(rule_);
        return rule_->entry().matchNext(text, offset, production);
    }

    long matchLength() const override
    {
        return rule_->entry().matchLength();
    }

    List<String> explain() const override
    {
        return List<String>{} << "Inline{" << str(static_cast<const void *>(rule_)) << "}";
    }

    const SyntaxRule *rule_ { nullptr };
};

InlineNode::InlineNode(const SyntaxRule *rule):
    SyntaxNode{new State{rule}}
{}

} // namespace cc::syntax
