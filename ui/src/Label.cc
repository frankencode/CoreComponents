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

    textRun->bind([=]{ return TextRun::create(text(), font()); });

    size->bind([=]{
        return
            2 * margin() +
            Size {
                textRun()->size()[0],
                textRun()->maxAscender() - textRun()->minDescender()
            };
    });

    if (font_->ink())
        ink = font_->ink();
    else
        ink->bind([=]{ return style()->theme()->primaryTextColor(); });

    textRun->connect([=]{ update(); });
    ink->connect([=]{ update(); });
}

Label::~Label()
{}

Point Label::textPos() const
{
    return
        center() -
        0.5 * Size {
            textRun()->size()[0],
            textRun()->maxAscender() - textRun()->minDescender()
        } +
        Size {
            0,
            textRun()->maxAscender()
        };
}

void Label::paint()
{
    Painter p(this);
    if (ink()) p->setSource(ink());
    p->showTextRun(textPos(), textRun());
}

}} // namespace cc::ui
