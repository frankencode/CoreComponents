/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/debug> // DEBUG
#include <cc/ui/TextRun>
#include <cc/ui/StylePlugin>
#include <cc/ui/Label>

namespace cc {
namespace ui {

Ref<Label> Label::create(View *parent, String text, const TextStyle *textStyle)
{
    return new Label(parent, text, textStyle);
}

Label::Label(View *parent, String text_, const TextStyle *textStyle_):
    View(parent),
    text(text_)
{
    if (textStyle_) textStyle = textStyle_;
    else textStyle = StylePlugin::instance()->defaultTextStyle();

    color = transparent;

    prepare();

    text     ->connect([=]{ prepare(); });
    textStyle->connect([=]{ prepare(); });
    size     ->connect([=]{ prepare(); });
}

void Label::prepare()
{
    textRun_ = TextRun::create(
        text(),
        textStyle()
    );
    size = Size { textRun_->advance()[0], textStyle()->font()->getMetrics()->ascender() } + 2 * StylePlugin::instance()->textMargin();

    update(UpdateReason::Changed);
}

void Label::paint()
{
    Painter(this)->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
