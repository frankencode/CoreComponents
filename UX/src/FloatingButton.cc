/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/FloatingButton>
#include <cc/PushButtonState>

namespace cc {

struct FloatingButton::State final: public PushButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        PushButton::State{text == "" ? Style::Floating : Style::FloatingWithLabel, text, icon}
    {}
};

FloatingButton::FloatingButton(Out<FloatingButton> self):
    PushButton{onDemand<State>}
{
    View::associate<FloatingButton>(self);
}

FloatingButton::FloatingButton(const Picture &icon, Out<FloatingButton> self):
    PushButton{new State{String{}, icon}}
{
    View::associate<FloatingButton>(self);
}

FloatingButton::FloatingButton(const String &text, Out<FloatingButton> self):
    PushButton{new State{text}}
{
    View::associate<FloatingButton>(self);
}

FloatingButton::FloatingButton(const String &text, const Picture &icon, Out<FloatingButton> self):
    PushButton{new State{text, icon}}
{
    View::associate<FloatingButton>(self);
}

FloatingButton &FloatingButton::associate(Out<FloatingButton> self)
{
    return View::associate<FloatingButton>(self);
}

} // namespace cc
