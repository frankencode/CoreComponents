/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/ui/Application>
#include <cc/ui/TextRun>
#include <cc/ui/Label>

namespace cc {
namespace ui {

Label::Label(View *parent, const String &text_, const Font &font_):
    View(parent),
    text(text_)
{
    inheritColor();

    if (font_)
        font = font_;
    else
        font->bind([=]{ return app()->defaultFont(); });

    textRun->bind([=]{ return TextRun::create(text(), font()); });
    textPos->bind([=]{ return center() + 0.5 * Step { -textRun()->size()[0], textRun()->size()[1] }; });
    margin->bind([=]{ return textRun()->minMargin(); });
    size->bind([=]{ return textRun()->size() + 2 * margin(); });
    ink->bind([=]{ return style()->theme()->primaryTextColor(); });

    textRun->connect([=]{ update(); });
    ink->connect([=]{ update(); });
}

Label::~Label()
{}

void Label::paint()
{
    Painter p(this);
    if (ink()) p->setSource(ink());
    p->showTextRun(textPos(), textRun());
}

}} // namespace cc::ui
