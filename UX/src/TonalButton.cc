/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TonalButton>
#include <cc/TouchButtonState>

namespace cc {

struct TonalButton::State final: public TouchButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        TouchButton::State{Style::Tonal, text, icon}
    {}
};

TonalButton::TonalButton(Out<TonalButton> self):
    TouchButton{onDemand<State>}
{
    View::associate<TonalButton>(self);
}

TonalButton::TonalButton(const String &text, const Picture &icon, Out<TonalButton> self):
    TouchButton{new State{text, icon}}
{
    View::associate<TonalButton>(self);
}

TonalButton &TonalButton::associate(Out<TonalButton> self)
{
    return View::associate<TonalButton>(self);
}

} // namespace cc
