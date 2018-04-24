/*
 * Copyright (C) 2018 Frank Mertens.
 *
 * Distribution and use is allowed under the terms of the zlib license
 * (see cc/LICENSE-zlib).
 *
 */

#include <cc/ui/Label>

namespace cc {
namespace ui {

Label *Label::create(View *parent, String text)
{
    return Object::create<Label>(parent, text);
}

Label::Label(View *parent, String text_):
    TextLine(parent, text_)
{
    margin->bind([=]{ return style()->defaultMargin(font()->size()); });
    ink->bind([=]{ return style()->theme()->primaryTextColor(); });
}

}} // namespace cc::ui
