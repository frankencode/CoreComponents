/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the GNU General Public License version 3
 * (see CoreComponents/LICENSE-gpl-3.0).
 *
 */

#include <cc/FilledButton>
#include <cc/TouchButtonState>

namespace cc {

struct FilledButton::State final: public TouchButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        TouchButton::State{Style::Filled, text, icon}
    {}
};

FilledButton::FilledButton(Out<FilledButton> self):
    TouchButton{onDemand<State>}
{
    View::associate<FilledButton>(self);
}

FilledButton::FilledButton(const String &text, Out<FilledButton> self):
    TouchButton{new State{text}}
{
    View::associate<FilledButton>(self);
}

FilledButton::FilledButton(const String &text, const Picture &icon, Out<FilledButton> self):
    TouchButton{new State{text, icon}}
{
    View::associate<FilledButton>(self);
}

FilledButton &FilledButton::associate(Out<FilledButton> self)
{
    return View::associate<FilledButton>(self);
}

} // namespace cc
