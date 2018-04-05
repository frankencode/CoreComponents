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

Ref<Label> Label::create(View *parent, String text, const Font &font)
{
    return Object::create<Label>(parent, text, font);
}

Label::Label(View *parent, String text_, const Font &font_):
    View(parent),
    text(text_)
{
    font->bind([=]{ return font_ * app()->textZoom(); });
    margin->bind([=]{ return style()->defaultMargin() * app()->textZoom(); });

    color = transparent;

    updateLayout();

    text  ->connect([=]{ updateLayout(); });
    font  ->connect([=]{ updateLayout(); });
    margin->connect([=]{ updateSize(); });
}

void Label::updateLayout()
{
    textRun_ = TextRun::create(text(), font());
    updateSize();
}

void Label::updateSize()
{
    size = textRun_->size() + 2 * margin();
    update();
}

void Label::paint()
{
    Painter(this)->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
