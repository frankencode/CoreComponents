/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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

FloatingButton::FloatingButton():
    PushButton{onDemand<State>}
{}

FloatingButton::FloatingButton(const Picture &icon):
    PushButton{new State{String{}, icon}}
{}

FloatingButton::FloatingButton(const String &text, const Picture &icon):
    PushButton{new State{text, icon}}
{}

FloatingButton &FloatingButton::associate(Out<FloatingButton> self)
{
    return View::associate<FloatingButton>(self);
}

} // namespace cc
