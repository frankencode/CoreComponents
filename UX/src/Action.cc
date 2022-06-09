/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Action>

namespace cc {

struct Action::State final: public Entity::State
{
    void invoke() const {
        if (action_) action_();
    }

    Property<String> title;
    Property<Shortcut> shortcut;
    Picture icon_;
    Fun<void()> action_;
};

Action::Action():
    Entity{onDemand<State>}
{}

String Action::title() const
{
    return me().title();
}

Action &Action::title(const String &newValue)
{
    me().title(newValue);
    return *this;
}

Action &Action::title(Definition<String> &&f)
{
    me().title(move(f));
    return *this;
}

Shortcut Action::shortcut() const
{
    return me().shortcut();
}

Action &Action::shortcut(const Shortcut &newValue)
{
    me().shortcut(newValue);
    return *this;
}

Action &Action::shortcut(Definition<Shortcut> &&f)
{
    me().shortcut(move(f));
    return *this;
}

Picture Action::icon() const
{
    return me().icon_;
}

Action &Action::icon(const Picture &newValue)
{
    me().icon_ = newValue;
    return *this;
}

Action &Action::onTriggered(Fun<void()> &&action)
{
    me().action_ = move(action);
    return *this;
}

void Action::operator()() const
{
    me().invoke();
}

Action::State &Action::me()
{
    return get<State>();
}

const Action::State &Action::me() const
{
    return get<State>();
}

} // namespace cc
