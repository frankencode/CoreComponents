/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
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

OutlineButton::OutlineButton():
    TouchButton{onDemand<State>}
{}

OutlineButton::OutlineButton(const String &text, const Picture &icon):
    TouchButton{new State{text, icon}}
{}

OutlineButton &OutlineButton::associate(Out<OutlineButton> self)
{
    return View::associate<OutlineButton>(self);
}

} // namespace cc
