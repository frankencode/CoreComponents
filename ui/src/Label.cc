/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/StylePlugin>
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
    text(text_),
    font(font_)
{
    if (font() == Font())
        font->bind([=]{ return StylePlugin::instance()->defaultFont(); });

    margin->bind([=]{ return StylePlugin::instance()->defaultMargin(); });

    color = transparent;

    updateLayout();

    text  ->connect([=]{ updateLayout(); });
    font  ->connect([=]{ updateLayout(); });
    margin->connect([=]{ updateLayout(); });
}

void Label::updateLayout()
{
    textRun_ = TextRun::create(text(), font());

    updateSize();
    update();
}

void Label::updateSize()
{
    size = textRun_->size() + 2 * margin();
}

void Label::paint()
{
    Painter(this)->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
