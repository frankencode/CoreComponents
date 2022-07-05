/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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

FilledButton::FilledButton():
    TouchButton{onDemand<State>}
{}

FilledButton::FilledButton(const String &text, const Picture &icon):
    TouchButton{new State{text, icon}}
{}

FilledButton &FilledButton::associate(Out<FilledButton> self)
{
    return View::associate<FilledButton>(self);
}

} // namespace cc
