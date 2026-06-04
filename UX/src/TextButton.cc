/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the Apache License version 2.0
 * (see CoreComponents/LICENSE-Apache-2.0).
 *
 */

#include <cc/TextButton>
#include <cc/TouchButtonState>

namespace cc {

struct TextButton::State final: public TouchButton::State
{
    explicit State(const String &text = String{}, const Picture &icon = Picture{}):
        TouchButton::State{Style::Text, text, icon}
    {}
};

TextButton::TextButton(Out<TextButton> self):
    TouchButton{onDemand<State>}
{
    View::associate<TextButton>(self);
}

TextButton::TextButton(const String &text, Out<TextButton> self):
    TouchButton{new State{text}}
{
    View::associate<TextButton>(self);
}

TextButton::TextButton(const Picture &icon, Out<TextButton> self):
    TouchButton{new State{String{}, icon}}
{
    View::associate<TextButton>(self);
}

TextButton::TextButton(const String &text, const Picture &icon, Out<TextButton> self):
    TouchButton{new State{text, icon}}
{
    View::associate<TextButton>(self);
}

TextButton &TextButton::associate(Out<TextButton> self)
{
    return View::associate<TextButton>(self);
}

} // namespace cc
