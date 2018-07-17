/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Application>
#include <cc/ui/TextRun>
#include <cc/ui/Label>

namespace cc {
namespace ui {

Label::Label(View *parent, const String &text_, const Font &font_):
    View(parent),
    text(text_)
{
    if (font_)
        font = font_;
    else
        font->bind([=]{ return app()->defaultFont(); });

    if (font_->paper())
        paper = font_->paper();
    else
        inheritPaper();

    textRun->bind([=]{ return TextRun::createHtml(text(), font()); });

    size->bind([=]{ return preferredSize(); });

    if (font_->ink())
        ink = font_->ink();
    else
        ink->bind([=]{ return style()->theme()->primaryTextColor(); });
}

Label::~Label()
{}

Point Label::textPos(double relativeHeight) const
{
    return
        center() -
        0.5 * Size {
            textRun()->size()[0],
            textRun()->maxAscender() - textRun()->minDescender()
        } +
        Size {
            0,
            textRun()->maxAscender() * (1 - relativeHeight)
        };
}

Size Label::preferredSize(const String &text, const Font &font, Size margin)
{
    return preferredSize(TextRun::createHtml(text, font), margin);
}

Size Label::preferredSize(const TextRun *textRun, Size margin)
{
    Size size =
        2 * margin +
        Size {
            textRun->size()[0],
            textRun->maxAscender() - textRun->minDescender()
        };
    return Size{ std::ceil(size[0]), std::ceil(size[1]) };
}

Size Label::preferredSize() const
{
    return preferredSize(textRun(), margin());
}

Size Label::minSize() const
{
    return preferredSize();
}

void Label::paint()
{
    Painter p(this);
    if (ink()) p->setSource(ink());
    p->showTextRun(textPos(), textRun());
}

}} // namespace cc::ui
