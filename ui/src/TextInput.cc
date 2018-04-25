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

TextInput *TextInput::create(View *parent)
{
    return Object::create<TextInput>(parent);
}

TextInput::TextInput(View *parent):
    Control(parent),
    timer_(Timer::create(0.5))
{
    color = Color::Transparent;

    font->bind([=]{ return app()->defaultFont(); });

    text  ->connect([=]{ updateLayout(); });
    font  ->connect([=]{ updateLayout(); });
    ink   ->connect([=]{ update(); });
    margin->connect([=]{ updateSize(); });

    updateLayout();
}

TextInput::~TextInput()
{}

void TextInput::updateLayout()
{
    textRun_ = TextRun::create(text(), font());
    updateSize();
}

void TextInput::updateSize()
{
    size = Size {
        textRun_->size()[0],
        std::ceil(font()->getMetrics()->ascender())
    } + 2 * margin();

    update();
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
    p->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
