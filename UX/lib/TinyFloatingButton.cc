/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/TinyFloatingButton>
#include <cc/PushButtonState>

namespace cc {

struct TinyFloatingButton::State final: public PushButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        PushButton::State{Style::TinyFloating, text, icon}
    {}
};

TinyFloatingButton::TinyFloatingButton():
    PushButton{onDemand<State>}
{}

TinyFloatingButton::TinyFloatingButton(const Picture &icon):
    PushButton{new State{String{}, icon}}
{}

TinyFloatingButton &TinyFloatingButton::associate(Out<TinyFloatingButton> self)
{
    return View::associate<TinyFloatingButton>(self);
}

} // namespace cc
