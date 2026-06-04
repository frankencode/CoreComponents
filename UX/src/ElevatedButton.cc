/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/ElevatedButton>
#include <cc/PushButtonState>

namespace cc {

struct ElevatedButton::State final: public PushButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        PushButton::State{Style::Elevated, text, icon}
    {}
};

ElevatedButton::ElevatedButton(Out<ElevatedButton> self):
    PushButton{onDemand<State>}
{
    View::associate<ElevatedButton>(self);
}

ElevatedButton::ElevatedButton(const Picture &icon, Out<ElevatedButton> self):
    PushButton{new State{String{}, icon}}
{
    View::associate<ElevatedButton>(self);
}

ElevatedButton::ElevatedButton(const String &text, Out<ElevatedButton> self):
    PushButton{new State{text}}
{
    View::associate<ElevatedButton>(self);
}

ElevatedButton::ElevatedButton(const String &text, const Picture &icon, Out<ElevatedButton> self):
    PushButton{new State{text, icon}}
{
    View::associate<ElevatedButton>(self);
}

ElevatedButton &ElevatedButton::associate(Out<ElevatedButton> self)
{
    return View::associate<ElevatedButton>(self);
}

} // namespace cc
