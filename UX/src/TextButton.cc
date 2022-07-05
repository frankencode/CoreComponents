/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
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

TextButton::TextButton():
    TouchButton{onDemand<State>}
{}

TextButton::TextButton(const String &text, const Picture &icon):
    TouchButton{new State{text, icon}}
{}

TextButton::TextButton(const Picture &icon):
    TouchButton{new State{String{}, icon}}
{}

TextButton &TextButton::associate(Out<TextButton> self)
{
    return View::associate<TextButton>(self);
}

} // namespace cc
