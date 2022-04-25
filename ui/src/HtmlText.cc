/*
 * Copyright (C) 2022 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/HtmlText>
#include <cc/ui/TextState>

namespace cc::ui {

struct HtmlText::State final: public Text::State
{
    explicit State(const String &initialText = String{}, Font initialFont = Font{}):
        Text::State{initialText, initialFont, true}
    {}
};

HtmlText::HtmlText():
    Text{onDemand<State>}
{}

HtmlText::HtmlText(Out<Text> self):
    Text{new State}
{
    self = weak<HtmlText>();
}

HtmlText::HtmlText(const String &text, Out<Text> self):
    Text{new State{text}}
{
    self = weak<HtmlText>();
}

HtmlText::HtmlText(const String &text, Font font, Out<Text> self):
    Text{new State{text, font}}
{
    self = weak<HtmlText>();
}

} // namespace cc::ui
