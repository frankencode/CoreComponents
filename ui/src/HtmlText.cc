/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/HtmlText>
#include <cc/TextState>

namespace cc {

struct HtmlText::State final: public Text::State
{
    explicit State(const String &initialText = String{}, Font initialFont = Font{}):
        Text::State{initialText, initialFont, true}
    {}
};

HtmlText::HtmlText():
    Text{onDemand<State>}
{}

HtmlText::HtmlText(const String &text, Font font):
    Text{new State{text, font}}
{}

HtmlText &HtmlText::associate(Out<HtmlText> self)
{
    return View::associate<HtmlText>(self);
}

} // namespace cc
