/*
 * Copyright (C) 2007-2016 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/TextBlock>

namespace cc {
namespace ui {

TextBlock::TextBlock(String text, TextStyle *style):
    text_(text),
    style_(style),
    interactive_(false)
{
    if (!style_) style_ = TextStyle::create();
}

}} // namespace cc::ui
