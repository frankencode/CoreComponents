/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/syntax/KeywordNode>

namespace cc::syntax {

struct KeywordNode::State: public SyntaxNode::State
{
    State(const Set<String> &keywords):
        keywords_{keywords}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        if (text.has(offset)) {
            Prefix prefix{text, offset};
            Locator pos;
            if (keywords_.find(prefix, &pos))
                offset += keywords_.at(pos).count();
            else
                offset = -1;
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
        List<String> parts;
        parts << "Keyword{";
        for (const String &keyword: keywords_) {
            parts << "\"" << keyword << "\",";
        }
        parts << "}";
        return parts;
    }

    Set<String> keywords_;
};

KeywordNode::KeywordNode(std::initializer_list<String> keywords):
    SyntaxNode{new State{keywords}}
{}

Set<String> KeywordNode::keywords() const
{
    return me().keywords_;
}

const KeywordNode::State &KeywordNode::me() const
{
    return Object::me.as<State>();
}

} // namespace cc::syntax
