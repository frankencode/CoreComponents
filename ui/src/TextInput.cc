/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug>
#include <cc/ui/TextRun>
#include <cc/ui/Timer>
#include <cc/ui/TextInput>

namespace cc {
namespace ui {

TextInput::TextInput(View *parent):
    Control(parent),
    timer_(Timer::create(0.5))
{
    inheritColor();

    font->bind([=]{ return app()->defaultFont(); });

    textRun->bind([=]{ return TextRun::create(text(), font()); });

    size->bind([=]{
        return Size {
            parent->size()[0],
            textRun()->size()[1] + 2 * margin()[1]
        };
    });

    ink->bind([=]{ return style()->theme()->primaryTextColor(); });

    textRun->connect([=]{ update(); });
    ink->connect([=]{ update(); });

    cursor->bind([=]{ return Cursor::create(focus() ? CursorShape::IBeam : CursorShape::Hand); });
}

TextInput::~TextInput()
{}

Point TextInput::textPos() const
{
    double a = std::ceil(font()->getMetrics()->ascender());
    return margin() + Point { 0, a };
}

bool TextInput::onPointerClicked(const PointerEvent *event)
{
    if (!focus())
        acquireTextInputFocus();

    return true;
}

void TextInput::paint()
{
    Painter p(this);
    if (ink()) p->setSource(ink());
    p->showTextRun(textPos(), textRun());
}

}} // namespace cc::ui
