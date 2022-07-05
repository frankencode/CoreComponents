/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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

ElevatedButton::ElevatedButton():
    PushButton{onDemand<State>}
{}

ElevatedButton::ElevatedButton(const Picture &icon):
    PushButton{new State{String{}, icon}}
{}

ElevatedButton::ElevatedButton(const String &text, const Picture &icon):
    PushButton{new State{text, icon}}
{}

ElevatedButton &ElevatedButton::associate(Out<ElevatedButton> self)
{
    return View::associate<ElevatedButton>(self);
}

} // namespace cc
