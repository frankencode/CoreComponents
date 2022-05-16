/*
 * Copyright (C) 2021 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Token>
#include <cc/Map>
#include <cc/TokenScreen>

namespace cc {

struct Token::MatchState
{
    Map<String, String> captures_;
};

String Token::State::capturedValue(const String &name) const
{
    const State *root = getRoot();
    if (!root) return String{};
    return root->matchState_->captures_.value(name);
}

void Token::State::setCapturedValue(const String &name, const String &value)
{
    const State *root = getRoot();
    if (!root->matchState_) root->matchState_ = new MatchState;
    root->matchState_->captures_.establish(name, value);
}

void Token::State::deleteMatchState()
{
    delete matchState_;
}

bool Token::project(const TokenScreen &screen) const
{
    TokenScreen target = screen;
    return projectCascade(target);
}

bool Token::projectCascade(TokenScreen &screen) const
{
    long i = range()[0];

    for (const Token &child: children())
    {
        if (i < child.range()[0]) {
            if (!screen.project(*this, i, child.range()[0]))
                return false;
            i = child.range()[0];
        }
        if (!child.projectCascade(screen))
            return false;

        i = child.range()[1];
    }

    if (i < range()[1]) {
        if (!screen.project(*this, i, range()[1]))
            return false;
    }

    return true;
}

} // namespace cc
