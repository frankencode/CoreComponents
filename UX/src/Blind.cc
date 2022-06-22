/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/Blind>
#include <cc/Application>

namespace cc {

class Blind::State final: public View::State
{
public:
    State()
    {
        size([this]{ return hasParent() ? parent().size() : Size{}; });
        paper([this]{ return theme().dialogScrimColor(); });
    }
};

Blind::Blind():
    View{onDemand<State>}
{}

Blind &Blind::associate(Out<Blind> self)
{
    return View::associate<Blind>(self);
}

} // namespace cc
