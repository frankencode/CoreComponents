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

TextLine *TextLine::create(View *parent, String text)
{
    return Object::create<TextLine>(parent, text);
}

TextLine::TextLine(View *parent, String text_):
    View(parent),
    text(text_)
{
    color = Color::Transparent;

    font->bind([=]{ return app()->defaultFont(); });

    text  ->connect([=]{ updateLayout(); });
    font  ->connect([=]{ updateLayout(); });
    ink   ->connect([=]{ update(); });
    margin->connect([=]{ updateSize(); });

    updateLayout();
}

TextLine::~TextLine()
{}

void TextLine::updateLayout()
{
    textRun_ = TextRun::create(text(), font());
    if (!updateSize())
        update();
}

bool TextLine::updateSize()
{
    Size newSize = textRun_->size() + 2 * margin();
    if (size() != newSize) {
        size = newSize;
        return true;
    }
    return false;
}

void TextLine::paint()
{
    Painter p(this);
    if (ink()) p->setSource(ink());
    p->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
