/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Label>
#include <cc/ui/Application>
#include <cc/ui/TextRun>
#include <cc/DEBUG>

namespace cc {
namespace ui {

Label::Instance::Instance(const String &initialText, const Font &initialFont):
    text{initialText}
{
    if (initialFont) {
        font = initialFont;
        if (initialFont->paper())
            paper = initialFont->paper();
    }
    else
        font <<[=]{ return Application{}->defaultFont(); };

    if (!paper()) paper <<[=]{ return basePaper(); };

    textRun <<[=]{ return TextRun::createHtml(text(), font()); };

    size <<[=]{ return preferredSize(); };

    if (initialFont->ink())
        ink = initialFont->ink();
    else
        ink <<[=]{ return style()->theme()->primaryTextColor(); };

    paint >>[=]{
        Painter p{this};
        if (ink()) p->setSource(ink());
        p->showTextRun(textPos(), textRun());
    };
}

Label::Instance::~Instance()
{}

Point Label::Instance::textPos(double relativeHeight) const
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

Size Label::Instance::preferredSize(const String &text, const Font &font, Size margin)
{
    return preferredSize(TextRun::createHtml(text, font), margin);
}

Size Label::Instance::preferredSize(const TextRun *textRun, Size margin)
{
    Size size =
        2 * margin +
        Size {
            textRun->size()[0],
            textRun->maxAscender() - textRun->minDescender()
        };
    return Size{ std::ceil(size[0]), std::ceil(size[1]) };
}

Size Label::Instance::preferredSize() const
{
    return preferredSize(textRun(), margin());
}

Size Label::Instance::minSize() const
{
    return preferredSize();
}

}} // namespace cc::ui
