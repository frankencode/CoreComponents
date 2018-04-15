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

TextLine *TextLine::create(View *parent, String text, const Font &font)
{
    return Object::create<TextLine>(parent, text, font);
}

TextLine *TextLine::create(View *parent, const Font &font)
{
    return TextLine::create(parent, "", font);
}

TextLine::TextLine(View *parent, String text_, const Font &font_):
    View(parent),
    text(text_)
{
    font->bind([=]{ return font_ * app()->textZoom(); });
    // margin->bind([=]{ return style()->defaultMargin(font()->size()); });

    color = Color::Transparent;

    updateLayout();

    text  ->connect([=]{ updateLayout(); });
    font  ->connect([=]{ updateLayout(); });
    margin->connect([=]{ updateSize(); });
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
    Painter(this)->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
