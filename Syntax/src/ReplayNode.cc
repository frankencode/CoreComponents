/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax_node/ReplayNode>
#include <cc/Token>

namespace cc::syntax {

struct ReplayNode::State: public SyntaxNode::State
{
    State(const String &name):
        name_{name}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        String scribble = production.capturedValue(name_);
        long j = 0;
        long m = scribble.count();
        for (long i = offset, n = text.count(); i < n && j < m; ++i) {
            if (text.at(i) != scribble.at(j)) break;
            ++j;
        }

        return (j < m) ? -1 : (offset + m);
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{}
            << "Replay{\"" << name_ << "\"}";
    }

    String name_;
};

ReplayNode::ReplayNode(const String &name):
    SyntaxNode{new State{name}}
{}

} // namespace cc::syntax
