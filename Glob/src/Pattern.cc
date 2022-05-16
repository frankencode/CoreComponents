/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Pattern>
#include <cc/PatternSyntax>

namespace cc {

struct Pattern::State final: public Object::State
{
    State(const String &text):
        definition_{&rule_},
        rule_{PatternSyntax{}.compile(text)},
        text_{text}
    {}

    SyntaxDefinition definition_;
    SyntaxRule rule_;
    String text_;
};

Pattern::Pattern(const String &text):
    Object{new State{text}}
{}

bool Pattern::match(const String &text, Out< List<Range> > captures) const
{
    Token token = me().definition_.match(text);
    if (captures.requested()) {
        captures = List<Range>{};
        if (token) {
            for (const Token &child: token.children()) {
                captures().append(child.range());
            }
        }
    }
    return token;
}

Range Pattern::findIn(const String &text, Out<long> offset) const
{
    Token token = me().definition_.findIn(text, offset());
    Range range;
    if (token) {
        offset = token.i0();
        range = token.range();
    }
    return range;
}

List<String> Pattern::breakUp(const String &text) const
{
    return me().definition_.breakUp(text);
}

long Pattern::matchLength() const
{
    return me().definition_.entry().matchLength();
}

List<String> Pattern::explain() const
{
    return me().rule_.explain();
}

String Pattern::text() const
{
    return me().text_;
}

const Pattern::State &Pattern::me() const
{
    return Object::me.as<State>();
}

} // namespace cc
