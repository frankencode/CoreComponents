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
