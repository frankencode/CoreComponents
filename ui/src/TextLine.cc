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
#include <cc/ui/TextLine>

namespace cc {
namespace ui {

TextLine *TextLine::create(View *parent, const String &text, const Font &font)
{
    return Object::create<TextLine>(parent, text, font);
}

TextLine::TextLine(View *parent, const String &text_, const Font &font_):
    View(parent),
    text(text_)
{
    if (parent && parent->color())
        color = parent->color();
    else
        color->bind([=]{ return style()->theme()->windowColor(); });

    if (font_)
        font = font_;
    else
        font->bind([=]{ return app()->defaultFont(); });

    textRun->bind([=]{ return TextRun::create(text(), font()); });
    margin->bind([=]{ return textRun()->minMargin(); });
    size->bind([=]{ return textRun()->size() + 2 * margin(); });

    textRun->connect([=]{ update(); });
    ink->connect([=]{ update(); });
}

TextLine::~TextLine()
{}

void TextLine::paint()
{
    Painter p(this);
    if (ink()) p->setSource(ink());
    Point pos = center() + 0.5 * Step { -textRun()->size()[0], textRun()->size()[1] };
    pos[0] = std::floor(pos[0]);
    pos[1] = std::floor(pos[1]);
    p->showTextRun(pos, textRun());
}

}} // namespace cc::ui
