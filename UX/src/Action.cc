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
    State(const String &initialTitle = String{}):
        title{initialTitle}
    {}

    Property<String> title;
    Property<Shortcut> shortcut;
    Property<Picture> icon;
    Trigger action_;
};

Action::Action():
    Entity{onDemand<State>}
{}

Action::Action(const String &title):
    Entity{new State{title}}
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
    return me().icon();
}

Action &Action::icon(const Picture &newValue)
{
    me().icon(newValue);
    return *this;
}

Action &Action::icon(Definition<Picture> &&f)
{
    me().icon(move(f));
    return *this;
}

Action &Action::onTriggered(Fun<void()> &&f)
{
    me().action_(move(f));
    return *this;
}

Action &Action::operator()(Fun<void()> &&f)
{
    me().action_(move(f));
    return *this;
}

void Action::operator()() const
{
    me().action_();
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
