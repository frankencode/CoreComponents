/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextRun>
#include <cc/ui/Timer>
#include <cc/ui/TextInput>

namespace cc {
namespace ui {

Ref<TextInput> TextInput::create(View *parent)
{
    return Object::create<TextInput>(parent);
}

TextInput::TextInput(View *parent):
    View(parent)
{
    color = Color::Transparent;
}

void TextInput::paint()
{
    Painter(this)->showTextRun(
        center() + 0.5 * Step { -textRun_->size()[0], textRun_->size()[1] },
        textRun_
    );
}

}} // namespace cc::ui
