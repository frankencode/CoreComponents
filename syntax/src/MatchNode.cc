/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/MatchNode>

namespace cc::syntax {

struct MatchNode::State: public SyntaxNode::State
{
    State(std::function<bool(char)> &&match):
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

    const std::function<bool(char)> match_;
};

MatchNode::MatchNode(std::function<bool(char)> &&match):
    SyntaxNode{new State{std::move(match)}}
{}

} // namespace cc::syntax
