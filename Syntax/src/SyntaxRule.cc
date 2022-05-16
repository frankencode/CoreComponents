/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/SyntaxRule>
#include <cc/syntax/VoidRule>
#include <cc/Token>

namespace cc {

struct SyntaxRule::State: public SyntaxNode::State
{
    State(const SyntaxNode &entry):
        entry_{entry}
    {}

    State(const String &name, const SyntaxNode &entry):
        entry_{entry},
        name_{name}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        Token token{&production};
        token.setRule(Object::alias<SyntaxRule>(this));

        Range range{offset};
        range[1] = entry_.matchNext(text, range[0], token);

        if (range[1] != -1) {
            token.setRange(range);
            production.children().emplaceBack(move(token));
        }

        return range[1];
    }

    long matchLength() const override
    {
        return entry_.matchLength();
    }

    List<String> explain() const override
    {
        return List<String>{} << "SyntaxRule{" << entry_.explain() << "}";
    }

    SyntaxNode entry_;
    Object metaData_;
    String name_;
};

SyntaxRule::SyntaxRule(const SyntaxNode &entry):
    SyntaxNode{new State{entry}}
{}

SyntaxRule::SyntaxRule(const String &name, const SyntaxNode &entry):
    SyntaxNode{new State{name, entry}}
{}

SyntaxRule::SyntaxRule(State *newState):
    SyntaxNode{newState}
{}

String SyntaxRule::name() const
{
    return me().name_;
}

void SyntaxRule::setName(const String &newName)
{
    me().name_ = newName;
}

syntax::SyntaxNode SyntaxRule::entry() const
{
    return me().entry_;
}

Object SyntaxRule::metaData() const
{
    return me().metaData_;
}

void SyntaxRule::setMetaData(const Object &newValue)
{
    me().metaData_ = newValue;
}

SyntaxRule::State &SyntaxRule::me()
{
    return Object::me.as<State>();
}

const SyntaxRule::State &SyntaxRule::me() const
{
    return Object::me.as<State>();
}

} // namespace cc

namespace cc::syntax {

struct VoidRule::State: public SyntaxRule::State
{
    State(const SyntaxNode &entry):
        SyntaxRule::State{entry}
    {}

    long matchNext(const String &text, long offset, Token &production) const override
    {
        Token::Children::Transaction transaction { production.children() };
        long newOffset = entry_.matchNext(text, offset, production);
        transaction.rollback();
        return newOffset;
    }

    List<String> explain() const override
    {
        return List<String>{} << "VoidRule{" << entry_.explain() << "}";
    }
};

VoidRule::VoidRule(const SyntaxNode &entry):
    SyntaxRule{new State{entry}}
{}

} // namespace cc::syntax
