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
    Property<String> title;
    Property<Shortcut> shortcut;
    Property<View> icon;
};

Action::Action():
    Entity{onDemand<State>}
{}

Action::Action(const String &title):
    Entity{new State}
{
    me().title(title);
}

Action::Action(const String &title, Shortcut shortcut):
    Entity{new State}
{
    me().title(title);
    me().shortcut(shortcut);
}

Action::Action(const View &icon):
    Entity{new State}
{
    me().icon(icon);
}

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

View Action::icon() const
{
    return me().icon();
}

Action &Action::icon(const View &newValue)
{
    me().icon(newValue);
    return *this;
}

Action &Action::icon(Definition<View> &&f)
{
    me().icon(move(f));
    return *this;
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
