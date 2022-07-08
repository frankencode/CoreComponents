/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/CaptureNode>
#include <cc/Token>

namespace cc::syntax {

struct CaptureNode::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry, const String &name):
        entry_{entry},
        name_{name}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        long h = entry_.matchNext(text, offset, production);
        if (h >= 0) production.setCapturedValue(name_, text.copy(offset, h));
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
            << "Capture{" << entry_.explain() << ",\"" << name_ << "\"}";
    }

    SyntaxNode entry_;
    String name_;
};

CaptureNode::CaptureNode(const SyntaxNode &entry, const String &name):
    SyntaxNode{new State{entry, name}}
{}

} // namespace cc::syntax
