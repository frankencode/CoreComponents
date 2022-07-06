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

class Blind::State final: public Control::State
{
public:
    State()
    {
        size([this]{ return hasParent() ? parent().size() : Size{}; });
        paper([this]{ return theme().dialogScrimColor(); });
    }

    bool feedFingerEvent(FingerEvent &event) const override
    {
        Control::State::feedFingerEvent(event);
        return true;
    }

    bool feedMouseEvent(MouseEvent &event) const override
    {
        Control::State::feedMouseEvent(event);
        return true;
    }

    bool feedWheelEvent(WheelEvent &event) const override
    {
        Control::State::feedWheelEvent(event);
        return true;
    }

    bool feedKeyEvent(KeyEvent &event) const override
    {
        Control::State::feedKeyEvent(event);
        return true;
    }
};

Blind::Blind():
    Control{onDemand<State>}
{}

Blind &Blind::associate(Out<Blind> self)
{
    return View::associate<Blind>(self);
}

} // namespace cc
