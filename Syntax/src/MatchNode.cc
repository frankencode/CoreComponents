/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/syntax/MatchNode>

namespace cc::syntax {

struct MatchNode::State: public SyntaxNode::State
{
    State(Fun<bool(char)> &&match):
        match_{match}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        if (text.has(offset)) {
            char ch = text.at(offset++);
            if (!match_(ch)) offset = -1;
        }
        else
            offset = -1;

        return offset;
    }

    long matchLength() const override
    {
        return -1;
    }

    List<String> explain() const override
    {
        return List<String>{} << "Match{...}";
    }

    const Fun<bool(char)> match_;
};

MatchNode::MatchNode(Fun<bool(char)> &&match):
    SyntaxNode{new State{move(match)}}
{}

} // namespace cc::syntax
