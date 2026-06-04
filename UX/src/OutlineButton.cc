/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/OutlineButton>
#include <cc/TouchButtonState>

namespace cc {

struct OutlineButton::State final: public TouchButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        TouchButton::State{Style::Outline, text, icon}
    {}
};

OutlineButton::OutlineButton(Out<OutlineButton> self):
    TouchButton{onDemand<State>}
{
    View::associate<OutlineButton>(self);
}

OutlineButton::OutlineButton(const String &text, Out<OutlineButton> self):
    TouchButton{new State{text}}
{
    View::associate<OutlineButton>(self);
}

OutlineButton::OutlineButton(const String &text, const Picture &icon, Out<OutlineButton> self):
    TouchButton{new State{text, icon}}
{
    View::associate<OutlineButton>(self);
}

OutlineButton &OutlineButton::associate(Out<OutlineButton> self)
{
    return View::associate<OutlineButton>(self);
}

} // namespace cc
